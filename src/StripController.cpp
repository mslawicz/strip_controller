#include "StripController.h"
#include "silabs_utils.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "sl_spidrv_instances.h"
#include <cstring>

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_DEV_SIZE     9   //number of pulse-coded bytes for each WS2812 device
#define WS2812_BUFFER_SIZE  (WS2812_DEV_SIZE * WS2812_NUMB_DEV)   //number of pulse-coded bytes for all WS2812 devices
#define ACTION_PERIOD   40  //action period 40 ms = 25 Hz

#define SC_EVENT_WAIT_FLAGS (SC_EVENT_TRANSMIT_REQ | \
                             SC_EVENT_COLOR_ACTION | \
                             SC_EVENT_LEVEL_ACTION | \
                             SC_EVENT_SET_HS_ACTION)                      

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
    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }
}

void stripControllerHandler(void* pvParameter)
{
    while(1)
    {
        //XXX test
        stripController.test();

        osDelay(SC_LOOP_PERIOD);
    }



/*
    uint32_t flags;

    while(1)
    {
        flags = osEventFlagsWait(stripControllerFlags, SC_EVENT_WAIT_FLAGS, osFlagsWaitAny, osWaitForever);

        if(flags & SC_EVENT_LEVEL_ACTION)
        {
            stripController.levelAction();
        }

        if(flags & SC_EVENT_COLOR_ACTION)
        {
            // device color handling action request
            stripController.colorAction();
        }

        if(flags & SC_EVENT_SET_HS_ACTION)
        {
            // set new fixed color from hue and saturation
            stripController.setColorHS();
        }

        if(flags & SC_EVENT_TRANSMIT_REQ)
        {
            // WS2812 data transmit request
            WS2812_transmit();
        }
    }
*/    
}

void StripController::turnOnOff(bool state)
{
    turnedOn = state;
    SILABS_LOG("StripController::turnOnOff -> %s", (turnedOn) ? "On" : "Off");

    //XXX test
    if(turnedOn == true)
    {
        GPIO_PinOutSet(test0_PORT, test0_PIN);
    }
    else
    {
        GPIO_PinOutClear(test0_PORT, test0_PIN);
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
            //osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
            WS2812_repeat = false;
        }
    }
}

void actionTimerCbk(void *arg)
{
    //osEventFlagsSet(stripControllerFlags, stripController.eventRequest);
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
    //osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
}

void StripController::levelAction(void)
{
    // if((currentLevel == targetLevel) || (levelTransitionSteps <2))
    // {
    //     currentLevel = targetLevel;
    //     levelTransitionSteps = 0;
    // }
    // else
    // {
    //     //transitional step
    //     int16_t nextLevelChange = (targetLevel - currentLevel) / levelTransitionSteps;
    //     currentLevel += nextLevelChange;
    //     levelTransitionSteps--;
    // }

    //set color action event to show the applied level changes
    //osEventFlagsSet(stripControllerFlags, SC_EVENT_COLOR_ACTION);
}

void StripController::setOnLevel(uint8_t newOnLevel)
{
    onLevel = newOnLevel;
    SILABS_LOG("StripController::setOnLevel -> %u", onLevel);
}

//set new fixed color from current hue and saturation values
void StripController::setColorHS(void)
{
    currentColorRGB = convertHStoRGB(currentColorHS);
    colorMode = ColorMode::FixedColor;
    //set color action event to show the applied fixed RGB color changes
    //osEventFlagsSet(stripControllerFlags, SC_EVENT_COLOR_ACTION);
}

void StripController::test(void)
{
    uint8_t level = onLevel;
    RGBToPulses(WS2812_buffer, RGB_t{0xFF, 0, 0}, level);
    RGBToPulses(WS2812_buffer + 9, RGB_t{0x7F, 0x7F, 0}, level);
    RGBToPulses(WS2812_buffer + 18, RGB_t{0, 0xFF, 0}, level);
    RGBToPulses(WS2812_buffer + 27, RGB_t{0, 0x7F, 0x7F}, level);
    RGBToPulses(WS2812_buffer + 36, RGB_t{0, 0, 0xFF}, level);
    RGBToPulses(WS2812_buffer + 45, RGB_t{0x7F, 0, 0x7F}, level);
    RGBToPulses(WS2812_buffer + 54, RGB_t{0x7F, 0x7F, 0x7F}, level);
    RGBToPulses(WS2812_buffer + 63, RGB_t{0x7F, 0x7F, 0x7F}, level);
    dataTransmit();
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
void StripController::RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint8_t level)
{
    //function for gamma correction of level
    static constexpr uint8_t MaxLevel = 255; //max level 255
    static constexpr uint8_t MaxLevel_2 = 127; //max level 255 halfed
    uint16_t correctedLevel = level * level / MaxLevel;     //corrected level in range <0,255>

    auto gammaCorrection = [&](uint8_t colorValue, uint8_t level) -> uint8_t
    {
        uint8_t correctedColorValue = static_cast<uint8_t>((colorValue * correctedLevel + MaxLevel_2) / MaxLevel);
        return ((correctedColorValue == 0) && (colorValue > 0)) ? 1 : correctedColorValue;
    };

    //WS2812 requires G-R-B order of bytes
    byteToPulses(pBuffer, gammaCorrection(RGB_data.G, level));
    byteToPulses(pBuffer + 3, gammaCorrection(RGB_data.R, level));
    byteToPulses(pBuffer + 6, gammaCorrection(RGB_data.B, level));
}

void StripController::setFixedColor(void)
{
    //set fixed color and level to all devices
    for(uint16_t dev = 0; dev < WS2812_NUMB_DEV; dev++)
    {
        RGBToPulses(params.pBuffer + dev * WS2812_DEV_SIZE, currentColorRGB, currentLevel);
    }
}

RGB_t StripController::convertHStoRGB(HueSat_t colorHS)
{
    RGB_t colorRGB;
    constexpr uint8_t NumbOfSectors = 3;    // number of sectors in the hue range
    constexpr RGB_t RGB_nodes[NumbOfSectors + 1] =
    {
        {0xFF, 0, 0},
        {0, 0xFF, 0},
        {0, 0, 0xFF},
        {0xFF, 0, 0}
    };
    constexpr uint8_t MaxHue = 254;     //max value of hue in calculations
    constexpr uint8_t HueSectorSize = (MaxHue + 1) / NumbOfSectors;  //size of the hue sector

    uint8_t hue = (colorHS.hue > MaxHue) ? MaxHue : colorHS.hue;    //hue in range <0,254>
    uint8_t idx = hue / HueSectorSize;  // index of hue sector <0,2>
    uint8_t hueSect = hue % HueSectorSize;  // hue value in a sector <0,HueSectorSize-1>
    uint8_t saturationFloor = (0xFF - colorHS.saturation) >> 1;     //saturation-derived component of RGB values
    // calculate RGB components from hue and saturation and RGB node array
    auto R = (RGB_nodes[idx].R + (RGB_nodes[idx + 1].R - RGB_nodes[idx].R) * hueSect / HueSectorSize) * colorHS.saturation / 0xFF + saturationFloor;
    auto G = (RGB_nodes[idx].G + (RGB_nodes[idx + 1].G - RGB_nodes[idx].G) * hueSect / HueSectorSize) * colorHS.saturation / 0xFF + saturationFloor;
    auto B = (RGB_nodes[idx].B + (RGB_nodes[idx + 1].B - RGB_nodes[idx].B) * hueSect / HueSectorSize) * colorHS.saturation / 0xFF + saturationFloor;
    colorRGB.R = static_cast<uint8_t>(R);
    colorRGB.G = static_cast<uint8_t>(G);
    colorRGB.B = static_cast<uint8_t>(B);
    return colorRGB;
}

void StripController::dataTransmit(void)
{
    SPIDRV_MTransmit(sl_spidrv_WS2812_handle, WS2812_buffer, WS2812_BUFFER_SIZE, WS2812_transferComplete);
}
