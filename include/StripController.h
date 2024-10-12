#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define SC_EVENT_ACTION_REQ         (1UL << 0)    //WS2812 data action request
#define SC_EVENT_TRANSMIT_REQ       (1UL << 1)    //WS2812 buffer transmit request

#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip

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
class StripController
{
    public:
    StripController(StripControllerParams_t& params);
    uint32_t action(void);

    private:
    StripControllerParams_t& params;
    void byteToPulses(uint8_t* pBuffer, uint8_t colorData);
    void RGBToPulses(uint8_t* pBuffer, RGB_t RGB_data);
};

void stripControllerTaskInit(void);