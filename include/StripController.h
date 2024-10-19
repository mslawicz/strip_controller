#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include <array>

#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip

#define SC_EVENT_TRANSMIT_REQ       (1UL << 0)    //WS2812 buffer transmit request
#define SC_EVENT_COLOR_ACTION       (1UL << 1)    //WS2812 color action request
#define SC_EVENT_LEVEL_ACTION       (1UL << 2)    //WS2812 level action request
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
    void turnOn(bool state) { turnedOn = state; }

    private:
    StripControllerParams_t& params;
    bool turnedOn{false};       //current on/off state
    ColorMode colorMode{ColorMode::FixedColor};
    uint16_t currentLevel{4000};    //device light current level (0-255) multiplied by 100
    uint16_t targetLevel{currentLevel};     //device light target level (0-255) multiplied by 100
    uint16_t levelTransitionSteps{0};
    RGB_t currentFixedColor{0xFF, 0xFF, 0xFF};
    std::array<RGB_t, WS2812_NUMB_DEV> RGB_buffer{0};
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint16_t level);
    void setFixedColor(void);
};

extern osEventFlagsId_t stripControllerFlags;
extern StripController stripController;

void stripControllerTaskInit(void);