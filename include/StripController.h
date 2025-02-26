#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip
#define SC_LOOP_PERIOD    40  //action period 40 ms = 25 Hz
#define WS2812_START_LEVEL 100    //level when the strip is turned on

struct StripControllerParams_t
{
    uint8_t* pBuffer = nullptr;     //strip components data buffer
    uint16_t numberOfDevices = 0;   //number of WS2812 devices in the strip
    uint8_t devSize = 0;            //number of pulse-coded bytes for each WS2812 device
    float handlerPeriod = 0.0;      //handler period in seconds
};

struct RGB_t
{
    uint8_t R;
    uint8_t G;
    uint8_t B;
};

struct HueSat_t
{
    uint8_t hue;
    uint8_t saturation;
};

enum class ColorMode : uint8_t
{
    FixedColor
};
class StripController
{
    public:
    StripController(StripControllerParams_t& params);
    bool transmitRequest{false};
    void setLevel(uint8_t newLevel);
    void turnOnOff(bool state);
    void dataTransmit(void);
    void setHue(uint8_t hue);
    void setSaturation(uint8_t saturation);
    void setColorTemperature(uint16_t colorTemperature);
    void verifyLevel(void);
    
    private:
    StripControllerParams_t& params;
    uint8_t currentLevel{0};    //current level of the strip
    uint8_t onLevel{WS2812_START_LEVEL};      //level when the strip is turned on
    bool turnedOn{false};       //current on/off state
    RGB_t bufferRGB[WS2812_NUMB_DEV];   //RGB data buffer for all devices
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint8_t level);
    ColorMode colorMode{ColorMode::FixedColor};
    RGB_t currentColorRGB{0x7F, 0x7F, 0x7F};
    HueSat_t currentColorHS{0, 0};
    void setFixedColor(void);
    RGB_t convertHStoRGB(HueSat_t colorHS);

};

extern StripController stripController;

void stripControllerTaskInit(void);