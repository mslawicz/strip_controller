#include "StripController.h"
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include <functional>

#include "sl_pwm.h" //XXX test
#include "sl_pwm_instances.h"   //XXX test

#define SC_TASK_STACK_SIZE (1024)

uint8_t striControllerStack[SC_TASK_STACK_SIZE];
osThread_t stripControllerTaskControlBlock;
constexpr osThreadAttr_t stripControllerTaskAttr = { .name = "StripControllerTask",
                                         .attr_bits  = osThreadDetached,
                                         .cb_mem     = &stripControllerTaskControlBlock,
                                         .cb_size    = osThreadCbSize,
                                         .stack_mem  = striControllerStack,
                                         .stack_size = SC_TASK_STACK_SIZE,
                                         .priority   = osPriorityBelowNormal };

osThreadId_t sStripControllerTaskHandle;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"

StripController& StripController::getInstance(void)
{ 
    StripController instance;
    return instance;
}

#pragma GCC diagnostic pop

void StripController::start(void)
{
    //XXX test of PWM
    sl_pwm_set_duty_cycle(&sl_pwm_WS2812_bit, 50);
    sl_pwm_start(&sl_pwm_WS2812_bit);

    //std::bind(&StripController::handler, this, nullptr);
    //sStripControllerTaskHandle = osThreadNew((osThreadFunc_t*)std::bind(&StripController::handler, &StripController::getInstance()), nullptr, &stripControllerTaskAttr);
}
