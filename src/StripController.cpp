#include "StripController.h"
#include "silabs_utils.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "sl_spidrv_instances.h"
#include <cstring>

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_BUFFER_SIZE  (9 * WS2812_NUMB_DEV)   //9 bytes for each WS2812 device

#define SC_EVENT_WAIT_FLAGS (SC_EVENT_ACTION_REQ | \
                             SC_EVENT_TRANSMIT_REQ)                      

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
osTimerId_t stripControllerTimer;
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
void stripControllerTimerCbk(void *arg);

void stripControllerTaskInit(void)
{
    stripControllerFlags = osEventFlagsNew(NULL);
    stripControllerTimer = osTimerNew(stripControllerTimerCbk, osTimerOnce, nullptr, nullptr);

    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }
    osTimerStart(stripControllerTimer, 20); //XXX test
}

void stripControllerHandler(void* pvParameter)
{
    uint32_t flags;

    while(1)
    {
        flags = osEventFlagsWait(stripControllerFlags, SC_EVENT_WAIT_FLAGS, osFlagsWaitAny, osWaitForever);

        if(flags & SC_EVENT_ACTION_REQ)
        {
            GPIO_PinOutSet(test0_PORT, test0_PIN);  //XXX test
            // device data action request
            uint32_t timeToNextAction = stripController.action();
            if(timeToNextAction != 0)
            {
                osTimerStart(stripControllerTimer, timeToNextAction); //next action will be called in timeToNextAction
            }
            GPIO_PinOutClear(test0_PORT, test0_PIN);  //XXX test
        }

        if(flags & SC_EVENT_TRANSMIT_REQ)
        {
            GPIO_PinOutSet(test1_PORT, test1_PIN);  //XXX test
            // WS2812 data transmit request
            WS2812_transmit();
            GPIO_PinOutClear(test1_PORT, test1_PIN);  //XXX test
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

void stripControllerTimerCbk(void *arg)
{
    osEventFlagsSet(stripControllerFlags, SC_EVENT_ACTION_REQ);
}

StripController::StripController(StripControllerParams_t& params) :
    params(params)
{

}

uint32_t StripController::action(void)
{
    uint8_t fc = 30;
    uint8_t hc = fc / 2;
    uint8_t qc = fc / 3;
    RGBToPulses(params.pBuffer, RGB_t{fc,0,0});     //XXX test red
    RGBToPulses(params.pBuffer+9, RGB_t{hc,hc,0});     //XXX test yellow
    RGBToPulses(params.pBuffer+18, RGB_t{0,fc,0});     //XXX test green
    RGBToPulses(params.pBuffer+27, RGB_t{0,hc,hc});     //XXX test cyan
    RGBToPulses(params.pBuffer+36, RGB_t{0,0,fc});     //XXX test blue
    RGBToPulses(params.pBuffer+45, RGB_t{hc,0,hc});     //XXX test magenta
    RGBToPulses(params.pBuffer+54, RGB_t{qc,qc,qc});     //XXX test gray
    RGBToPulses(params.pBuffer+63, RGB_t{qc,qc,qc});     //XXX test gray    

    osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
    return 20;
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

//codes 3 bytes of RGB color value into 9 bytes of WS2812 coded pulses at the address pBuffer
void StripController::RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data)
{
    //WS2812 requires G-R-B order of bytes
    byteToPulses(pBuffer, RGB_data.G);
    byteToPulses(pBuffer + 3, RGB_data.R);
    byteToPulses(pBuffer + 6, RGB_data.B);
}
