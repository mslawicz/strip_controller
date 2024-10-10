#pragma once

#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

class StripController
{
    public:
    static StripController& getInstance(void);

    private:
    StripController() = default;
};

void stripControllerTaskInit(void);