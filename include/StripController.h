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
    StripController(StripControllerParams_t& params);
    uint8_t targetLevel{0}; //target level: either onLevel or 0
    uint8_t currentLevel{0};    //current level of the strip
    void colorAction(void);
    void levelAction(void);
    void setOnLevel(uint8_t newOnLevel);
    void turnOnOff(bool state);
    void setHue(uint8_t hue) { currentColorHS.hue = hue; }
    void setSaturation(uint8_t saturation) { currentColorHS.saturation = saturation; }
    void setColorHS(void);
    void test(void);    //XXX test
    void dataTransmit(void);

    private:
    StripControllerParams_t& params;
    uint8_t onLevel{1};    //current ON level set from the Matter controller
    bool turnedOn{false};       //current on/off state
    ColorMode colorMode{ColorMode::FixedColor};
    uint16_t levelTransitionSteps{0};
    RGB_t currentColorRGB{0xFF, 0xFF, 0xFF};
    HueSat_t currentColorHS{0, 0};
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint8_t level);
    void setFixedColor(void);
    RGB_t convertHStoRGB(HueSat_t colorHS);
};

extern StripController stripController;

void stripControllerTaskInit(void);