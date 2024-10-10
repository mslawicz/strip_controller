#include "StripController.h"
#include "silabs_utils.h"

#include "sl_pwm.h" //XXX test
#include "sl_pwm_instances.h"   //XXX test
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test

#define SC_TASK_STACK_SIZE (1024)

uint8_t stripControllerStack[SC_TASK_STACK_SIZE];
osThread_t stripControllerTaskControlBlock;
constexpr osThreadAttr_t stripControllerTaskAttr =
{
    .name = "StripCtrl",
    .attr_bits  = osThreadDetached,
    .cb_mem     = &stripControllerTaskControlBlock,
    .cb_size    = osThreadCbSize,
    .stack_mem  = stripControllerStack,
    .stack_size = SC_TASK_STACK_SIZE,
    .priority   = osPriorityBelowNormal
};

void stripControllerHandler(void * pvParameter);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wreturn-local-addr"

StripController& StripController::getInstance(void)
{ 
    StripController instance;
    return instance;
}

#pragma GCC diagnostic pop

void stripControllerTaskInit(void)
{
    //XXX test of PWM
    sl_pwm_set_duty_cycle(&sl_pwm_WS2812_bit, 20);
    sl_pwm_start(&sl_pwm_WS2812_bit);

    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }
}

void stripControllerHandler(void* pvParameter)
{
    static uint8_t duty = 0;
    while(1)
    {
        osDelay(10);
        GPIO_PinOutToggle(test0_PORT, test0_PIN);
        sl_pwm_set_duty_cycle(&sl_pwm_WS2812_bit, duty != 0 ? 66 : 33);
        duty = 1 - duty;
    }
}
