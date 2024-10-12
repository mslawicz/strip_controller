#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

#define SC_EVENT_ACTION_REQ     (1 << 0)    //RGB LED action request

class StripController
{
    public:
    static StripController& getInstance(void);

    private:
    StripController() = default;
};

void stripControllerTaskInit(void);