#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip
#define SC_LOOP_PERIOD    40  //action period 40 ms = 25 Hz

struct StripControllerParams_t
{
    uint8_t* pBuffer = nullptr;     //strip components data buffer
    uint16_t numberOfDevices = 0;   //number of WS2812 devices in the strip
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
    volatile uint32_t eventRequest{0};   //event flags to be set in action timer callback
    StripController(StripControllerParams_t& params);
    void colorAction(void);
    void levelAction(void);
    void setTargetLevel(uint8_t newTargetLevel) { targetLevel = 100 * newTargetLevel; }
    void turnOnOff(bool state);
    void setHue(uint8_t hue) { currentColorHS.hue = hue; }
    void setSaturation(uint8_t saturation) { currentColorHS.saturation = saturation; }
    void setColorHS(void);

    private:
    StripControllerParams_t& params;
    bool turnedOn{false};       //current on/off state
    ColorMode colorMode{ColorMode::FixedColor};
    uint16_t currentLevel{4000};    //device light current level (0-255) multiplied by 100
    uint16_t targetLevel{currentLevel};     //device light target level (0-255) multiplied by 100
    uint16_t levelTransitionSteps{0};
    RGB_t currentColorRGB{0xFF, 0xFF, 0xFF};
    HueSat_t currentColorHS{0, 0};
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint16_t level);
    void setFixedColor(void);
    RGB_t convertHStoRGB(HueSat_t colorHS);
};

extern StripController stripController;

void stripControllerTaskInit(void);