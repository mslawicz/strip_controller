#include "StripController.h"
#include "silabs_utils.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "sl_spidrv_instances.h"
#include <cstring>

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip
#define WS2812_DEV_SIZE     9   //number of pulse-coded bytes for each WS2812 device
#define WS2812_BUFFER_SIZE  (WS2812_DEV_SIZE * WS2812_NUMB_DEV)   //number of pulse-coded bytes for all WS2812 devices
#define ACTION_PERIOD   40  //action period 40 ms = 25 Hz

#define SC_EVENT_WAIT_FLAGS (SC_EVENT_TRANSMIT_REQ | \
                             SC_EVENT_COLOR_ACTION | \
                             SC_EVENT_LEVEL_ACTION)                      

uint8_t stripControllerStack[SC_TASK_STACK_SIZE];
osThread_t stripControllerTaskControlBlock;
constexpr osThreadAttr_t stripControllerTaskAttr =
{
    .name = "StripCtrl",
    .attr_bits  = osThreadDetached,
    .cb_mem     = &stripControllerTaskControlBlock,
    .cb_size    = osThreadCbSize,
    .stack_mem  = stripControllerStack,
    .stack_size = SC_TASK_STACK_SIZE,
    .priority   = osPriorityBelowNormal
};

uint8_t WS2812_buffer[WS2812_BUFFER_SIZE];
static volatile bool WS2812_busy = false;
static volatile bool WS2812_repeat = false;
osTimerId_t actionTimer;
osEventFlagsId_t stripControllerFlags;
StripControllerParams_t stripControllerParams =
{
    .pBuffer = WS2812_buffer,
    .numberOfDevices = WS2812_NUMB_DEV
};
StripController stripController(stripControllerParams);

void stripControllerHandler(void * pvParameter);
void WS2812_transmit(void);
void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred);
void actionTimerCbk(void *arg);

void stripControllerTaskInit(void)
{
    stripControllerFlags = osEventFlagsNew(NULL);
    actionTimer = osTimerNew(actionTimerCbk, osTimerOnce, nullptr, nullptr);

    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }

    //init the first action to set the WS2812 devices
    stripController.eventRequest = SC_EVENT_COLOR_ACTION;
    osTimerStart(actionTimer, ACTION_PERIOD);
}

void stripControllerHandler(void* pvParameter)
{
    uint32_t flags;

    while(1)
    {
        flags = osEventFlagsWait(stripControllerFlags, SC_EVENT_WAIT_FLAGS, osFlagsWaitAny, osWaitForever);

        stripController.nextActionRequest = false;

        if(flags & SC_EVENT_COLOR_ACTION)
        {
            // device data action request
            stripController.colorAction();
        }

        if(flags & SC_EVENT_LEVEL_ACTION)
        {
            stripController.levelAction();
        }

        if(flags & SC_EVENT_TRANSMIT_REQ)
        {
            // WS2812 data transmit request
            WS2812_transmit();
        }

        if(stripController.nextActionRequest)
        {
            //some actions need another pass - start timer to trig the event
            osTimerStart(actionTimer, ACTION_PERIOD);
        }
    }
}

void WS2812_transmit(void)
{
    if(!WS2812_busy)
    {
        WS2812_busy = true;
        SPIDRV_MTransmit(sl_spidrv_WS2812_handle, WS2812_buffer, WS2812_BUFFER_SIZE, WS2812_transferComplete);
    }
    else
    {
        //request transmit repetition because of possible updated data
        WS2812_repeat = true;
    }      
}    

void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred)
{
    if (transferStatus == ECODE_EMDRV_SPIDRV_OK)
    {
        WS2812_busy = false;
        if(WS2812_repeat)
        {
            // set next transmit request
            osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
            WS2812_repeat = false;
        }
    }
}

void actionTimerCbk(void *arg)
{
    osEventFlagsSet(stripControllerFlags, stripController.eventRequest);
    stripController.eventRequest = 0;
}

StripController::StripController(StripControllerParams_t& params) :
    params(params)
{

}

void StripController::colorAction(void)
{
    switch(colorMode)
    {
        case ColorMode::FixedColor:
        setFixedColor();
        break;

        default:
        break;
    }

    //set transmit event to show the applied color changes
    osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
}

void StripController::levelAction(void)
{
    //set transmit event to show the applied level changes
    osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);

    if((currentLevel == targetLevel) || (levelTransitionSteps <2))
    {
        currentLevel = targetLevel;
        levelTransitionSteps = 0;
    }
    else
    {
        //transitional step
        int16_t nextLevelChange = (targetLevel - currentLevel) / levelTransitionSteps;
        currentLevel += nextLevelChange;
        levelTransitionSteps--;
        requestEvent(SC_EVENT_LEVEL_ACTION);
    }
}

//codes one byte of color value into 3 bytes of WS2812 coded pulses at the address pBuffer
void StripController::byteToPulses(uint8_t* pBuffer, uint8_t colorData)
{
    uint32_t pulseBuffer = 0;

    for(uint8_t bit = 0; bit < 8; bit++)
    {
        pulseBuffer <<= 3;
        //bit 0 is represented by the pulse codded with bits 100 (duty 33%)
        //bit 1 is represented by the pulse codded with bits 110 (duty 66%)
        pulseBuffer |= ((colorData << bit) & 0x80) == 0 ? 0x04 : 0x06;
    }

    //place bytes in the buffer in the big endian order
    *pBuffer = (pulseBuffer >> 16) & 0xFF;
    *(pBuffer + 1) = (pulseBuffer >> 8) & 0xFF;
    *(pBuffer + 2) = pulseBuffer & 0xFF;
}

//codes 3 bytes of RGB color value and level value into 9 bytes of WS2812 coded pulses at the address pBuffer
void StripController::RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint16_t level)
{
    //apply level gamma correction
    static constexpr uint16_t MaxLevel = 25500; //max level 255 multiplied by 100
    static constexpr uint16_t MaxLevel_2 = 12750; //max level 255 multiplied by 100 and halfed
    uint16_t correctedLevel = level * level / MaxLevel;     //corrected level in range <0,25500>
    //WS2812 requires G-R-B order of bytes
    byteToPulses(pBuffer, static_cast<uint8_t>((RGB_data.G * correctedLevel + MaxLevel_2)/ MaxLevel));
    byteToPulses(pBuffer + 3, static_cast<uint8_t>((RGB_data.R * correctedLevel + MaxLevel_2)/ MaxLevel));
    byteToPulses(pBuffer + 6, static_cast<uint8_t>((RGB_data.B * correctedLevel + MaxLevel_2)/ MaxLevel));
}

void StripController::setFixedColor(void)
{
    //set fixed color and level to all devices
    for(uint16_t dev = 0; dev < WS2812_NUMB_DEV; dev++)
    {
        RGBToPulses(params.pBuffer + dev * WS2812_DEV_SIZE, currentFixedColor, currentLevel);
    }
}

//request an event to be set by the callback function
void StripController::requestEvent(uint32_t flags)
{
    eventRequest |= flags;
    nextActionRequest = true; 
}
