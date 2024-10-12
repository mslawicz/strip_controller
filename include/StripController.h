#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define SC_EVENT_ACTION_REQ         (1UL << 0)    //WS2812 data action request
#define SC_EVENT_TRANSMIT_REQ       (1UL << 1)    //WS2812 buffer transmit request

struct StripControllerParams_t
{
    uint8_t* pBuffer = nullptr;
};  
class StripController
{
    public:
    StripController(StripControllerParams_t& params);
    uint32_t action(void);

    private:
    StripControllerParams_t& params;
};

void stripControllerTaskInit(void);