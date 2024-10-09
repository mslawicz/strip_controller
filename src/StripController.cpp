#include "StripController.h"

#include "sl_pwm.h" //XXX test
#include "sl_pwm_instances.h"   //XXX test

//initialize strip controller task
void stripControllerInit(void)
{
    //XXX test of PWM
    sl_pwm_set_duty_cycle(&sl_pwm_WS2812_bit, 50);
    sl_pwm_start(&sl_pwm_WS2812_bit);
}    