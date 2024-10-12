#include "StripController.h"
#include "silabs_utils.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "sl_spidrv_instances.h"

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_BUFFER_SIZE  9

#define SC_EVENT_WAIT_FLAGS (SC_EVENT_ACTION_REQ | \
                             SC_EVENT_TRANSMIT_REQ)

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
static volatile bool WS2812_busy = false;
static volatile bool WS2812_repeat = false;
osTimerId_t stripControllerTimer;
osEventFlagsId_t stripControllerFlags;

void stripControllerHandler(void * pvParameter);
void WS2812_transmit(void);
void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred);
void stripControllerTimerCbk(void *arg);

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
    stripControllerFlags = osEventFlagsNew(NULL);
    stripControllerTimer = osTimerNew(stripControllerTimerCbk, osTimerOnce, nullptr, nullptr);

    osThreadId_t stripControllerTaskHandle = osThreadNew(stripControllerHandler, nullptr, &stripControllerTaskAttr);
    if(stripControllerTaskHandle == 0)
    {
        SILABS_LOG("strip contrller task error");
    }
    osTimerStart(stripControllerTimer, 20); //XXX test
}

void stripControllerHandler(void* pvParameter)
{
    uint32_t flags;

    while(1)
    {
        flags = osEventFlagsWait(stripControllerFlags, SC_EVENT_WAIT_FLAGS, osFlagsWaitAny, osWaitForever);

        if(flags & SC_EVENT_ACTION_REQ)
        {
            GPIO_PinOutSet(test0_PORT, test0_PIN);  //XXX test
            // device data action request
            //XXX SPI test - send 9 bytes for one WS2812 device
            WS2812_buffer[0] = 0x9B;
            WS2812_buffer[1] = 0x49;
            WS2812_buffer[2] = 0xB4;
            WS2812_buffer[3] = 0x93;
            WS2812_buffer[4] = 0x4D;
            WS2812_buffer[5] = 0xA4;
            WS2812_buffer[6] = 0xDB;
            WS2812_buffer[7] = 0x69;
            WS2812_buffer[8] = 0xA6;            
            osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
            osTimerStart(stripControllerTimer, 20); //next event in 20 ms
            GPIO_PinOutClear(test0_PORT, test0_PIN);  //XXX test
        }

        if(flags & SC_EVENT_TRANSMIT_REQ)
        {
            GPIO_PinOutSet(test1_PORT, test1_PIN);  //XXX test
            // WS2812 data transmit request
            WS2812_transmit();
            GPIO_PinOutClear(test1_PORT, test1_PIN);  //XXX test
        }
    }
}

void WS2812_transmit(void)
{
    if(!WS2812_busy)
    {
        WS2812_busy = true;
        SPIDRV_MTransmit(sl_spidrv_WS2812_handle, WS2812_buffer, WS2812_BUFFER_SIZE, WS2812_transferComplete);
    }
    else
    {
        //request transmit repetition because of possible updated data
        WS2812_repeat = true;
    }      
}    

void WS2812_transferComplete(SPIDRV_Handle_t handle, Ecode_t transferStatus, int itemsTransferred)
{
    if (transferStatus == ECODE_EMDRV_SPIDRV_OK)
    {
        WS2812_busy = false;
        if(WS2812_repeat)
        {
            // set next transmit request
            osEventFlagsSet(stripControllerFlags, SC_EVENT_TRANSMIT_REQ);
            WS2812_repeat = false;
        }
    }
}

void stripControllerTimerCbk(void *arg)
{
    osEventFlagsSet(stripControllerFlags, SC_EVENT_ACTION_REQ);
}
