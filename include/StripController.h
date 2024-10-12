#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define SC_EVENT_ACTION_REQ         (1UL << 0)    //WS2812 data action request
#define SC_EVENT_TRANSMIT_REQ       (1UL << 1)    //WS2812 buffer transmit request

class StripController
{
    public:
    static StripController& getInstance(void);

    private:
    StripController() = default;
};

void stripControllerTaskInit(void);