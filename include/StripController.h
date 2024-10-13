#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

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
    bool nextActionRequest{false};
    uint32_t eventRequest{0};   //event flags to be set in action timer callback
    StripController(StripControllerParams_t& params);
    void colorAction(void);

    private:
    StripControllerParams_t& params;
    ColorMode colorMode{ColorMode::FixedColor};
    uint8_t currentLevel{30};
    RGB_t currentFixedColor{0xFF, 0xFF, 0xFF};
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data, uint8_t level);
    void setFixedColor(void);
};

void stripControllerTaskInit(void);