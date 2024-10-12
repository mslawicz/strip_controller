#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include <tuple>
#include <array>

#define SC_EVENT_ACTION_REQ         (1UL << 0)    //WS2812 data action request
#define SC_EVENT_TRANSMIT_REQ       (1UL << 1)    //WS2812 buffer transmit request

#define WS2812_NUMB_DEV     8   //number of WS2812 devices in the strip

struct StripControllerParams_t
{
    uint8_t* pBuffer = nullptr;     //strip components data buffer
    uint16_t numberOfDevices = 0;   //number of WS2812 devices in the strip
};

using RGB_t = std::tuple<uint8_t, uint8_t, uint8_t>;  
class StripController
{
    public:
    StripController(StripControllerParams_t& params);
    uint32_t action(void);

    private:
    StripControllerParams_t& params;
    std::array<RGB_t, WS2812_NUMB_DEV> RGB_array;
};

void stripControllerTaskInit(void);