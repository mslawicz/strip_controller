#include "StripController.h"
#include "silabs_utils.h"
#include "em_ldma.h"
#include "sl_pwm.h"
#include "sl_pwm_instances.h"
#include "em_gpio.h"    //XXX test
#include "pin_config.h" //XXX test
#include "em_timer.h"

#define SC_TASK_STACK_SIZE (1024)
#define WS2812_BIT_SIZE 5
/*(8 * 24 + 1)*/

/**
 * @brief
 *   DMA descriptor initializer for byte transfers from memory to a peripheral.
 * @param[in] src       Source data address.
 * @param[in] dest      Peripheral data register destination address.
 * @param[in] count     Number of bytes to transfer.
 */
#define LDMA_DESCRIPTOR_SINGLE_M2P_HALF(src, dest, count) \
  {                                                       \
    .xfer =                                               \
    {                                                     \
      .structType   = ldmaCtrlStructTypeXfer,             \
      .structReq    = 0,                                  \
      .xferCnt      = (count) - 1,                        \
      .byteSwap     = 0,                                  \
      .blockSize    = ldmaCtrlBlockSizeUnit1,             \
      .doneIfs      = 0,                                  \
      .reqMode      = ldmaCtrlReqModeBlock,               \
      .decLoopCnt   = 0,                                  \
      .ignoreSrec   = 0,                                  \
      .srcInc       = ldmaCtrlSrcIncOne,                  \
      .size         = ldmaCtrlSizeHalf,                   \
      .dstInc       = ldmaCtrlDstIncNone,                 \
      .srcAddrMode  = ldmaCtrlSrcAddrModeAbs,             \
      .dstAddrMode  = ldmaCtrlDstAddrModeAbs,             \
      .srcAddr      = (uint32_t)(src),                    \
      .dstAddr      = (uint32_t)(dest),                   \
      .linkMode     = 0,                                  \
      .link         = 0,                                  \
      .linkAddr     = 0                                   \
    }                                                     \
  }

static uint16_t WS2812_bits[WS2812_BIT_SIZE];   //PWM values of WS2812 bits
// Configure DMA transfer
const LDMA_TransferCfg_t ldmaPwmTimerCfg = LDMA_TRANSFER_CFG_PERIPHERAL(ldmaPeripheralSignal_TIMER2_CC0);
const LDMA_Descriptor_t ldmaPwmTimerDesc = LDMA_DESCRIPTOR_SINGLE_M2P_HALF(WS2812_bits, &TIMER2->CC[0].OCB, WS2812_BIT_SIZE);

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

static uint16_t WS2812_bit_0;
static uint16_t WS2812_bit_1;

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
    // Initialize the LDMA with default values
    LDMA_Init_t ldmaInit = LDMA_INIT_DEFAULT;
    LDMA_Init(&ldmaInit);
    
    //start PWM generation with duty 0 (no wave)
    sl_pwm_set_duty_cycle(&sl_pwm_WS2812_bit, 0);
    sl_pwm_start(&sl_pwm_WS2812_bit);

    //calculate PWM timer OC values for WS2812 bits
    uint32_t WS2812_timerTop = TIMER_TopGet((&sl_pwm_WS2812_bit)->timer);
    WS2812_bit_0 = static_cast<uint16_t>(WS2812_timerTop * 33 / 100);  // duty 33%
    WS2812_bit_1 = static_cast<uint16_t>(WS2812_timerTop * 66 / 100);  // duty 66%

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
        GPIO_PinOutToggle(test0_PORT, test0_PIN);

        WS2812_bits[0] = WS2812_bit_0;
        WS2812_bits[1] = WS2812_bit_1;
        WS2812_bits[2] = WS2812_bit_0;
        WS2812_bits[3] = WS2812_bit_1;
        WS2812_bits[4] = 0;
        LDMA_StartTransfer(0, &ldmaPwmTimerCfg, &ldmaPwmTimerDesc);
    }
}
