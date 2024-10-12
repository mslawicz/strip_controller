#include "StripController.h"
#include "silabs_utils.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "sl_spidrv_instances.h"

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_BUFFER_SIZE  4

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

uint8_t WS2812_buffer[WS2812_BUFFER_SIZE];

void stripControllerHandler(void * pvParameter);
void WS2812_transmit(void);
void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred);

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
    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }
}

void stripControllerHandler(void* pvParameter)
{
    while(1)
    {
        osDelay(10);

        //XXX SPI test
        WS2812_buffer[0] = 0x9B;
        WS2812_buffer[1] = 0x49;
        WS2812_buffer[2] = 0xB4;
        WS2812_buffer[3] = 0x00;
        WS2812_transmit();
    }
}

void WS2812_transmit(void)
{
    GPIO_PinOutSet(test0_PORT, test0_PIN);
    SPIDRV_MTransmit(sl_spidrv_WS2812_handle, WS2812_buffer, WS2812_BUFFER_SIZE, WS2812_transferComplete);
    GPIO_PinOutToggle(test0_PORT, test0_PIN);
    GPIO_PinOutToggle(test0_PORT, test0_PIN);
}    

void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred)
{
    if (transferStatus == ECODE_EMDRV_SPIDRV_OK)
    {
        GPIO_PinOutClear(test0_PORT, test0_PIN);
    }
}
