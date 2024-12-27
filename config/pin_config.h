#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// $[CMU]
// [CMU]$

// $[LFXO]
// [LFXO]$

// $[PRS.ASYNCH0]
// [PRS.ASYNCH0]$

// $[PRS.ASYNCH1]
// [PRS.ASYNCH1]$

// $[PRS.ASYNCH2]
// [PRS.ASYNCH2]$

// $[PRS.ASYNCH3]
// [PRS.ASYNCH3]$

// $[PRS.ASYNCH4]
// [PRS.ASYNCH4]$

// $[PRS.ASYNCH5]
// [PRS.ASYNCH5]$

// $[PRS.ASYNCH6]
// [PRS.ASYNCH6]$

// $[PRS.ASYNCH7]
// [PRS.ASYNCH7]$

// $[PRS.ASYNCH8]
// [PRS.ASYNCH8]$

// $[PRS.ASYNCH9]
// [PRS.ASYNCH9]$

// $[PRS.ASYNCH10]
// [PRS.ASYNCH10]$

// $[PRS.ASYNCH11]
// [PRS.ASYNCH11]$

// $[PRS.ASYNCH12]
// [PRS.ASYNCH12]$

// $[PRS.ASYNCH13]
// [PRS.ASYNCH13]$

// $[PRS.ASYNCH14]
// [PRS.ASYNCH14]$

// $[PRS.ASYNCH15]
// [PRS.ASYNCH15]$

// $[PRS.SYNCH0]
// [PRS.SYNCH0]$

// $[PRS.SYNCH1]
// [PRS.SYNCH1]$

// $[PRS.SYNCH2]
// [PRS.SYNCH2]$

// $[PRS.SYNCH3]
// [PRS.SYNCH3]$

// $[GPIO]
// [GPIO]$

// $[TIMER0]
// [TIMER0]$

// $[TIMER1]
// [TIMER1]$

// $[TIMER2]
// [TIMER2]$

// $[TIMER3]
// [TIMER3]$

// $[TIMER4]
// [TIMER4]$

// $[USART0]
// USART0 CLK on PA01
#ifndef USART0_CLK_PORT                         
#define USART0_CLK_PORT                          SL_GPIO_PORT_A
#endif
#ifndef USART0_CLK_PIN                          
#define USART0_CLK_PIN                           1
#endif

// USART0 RX on PA02
#ifndef USART0_RX_PORT                          
#define USART0_RX_PORT                           SL_GPIO_PORT_A
#endif
#ifndef USART0_RX_PIN                           
#define USART0_RX_PIN                            2
#endif

// USART0 TX on PA04
#ifndef USART0_TX_PORT                          
#define USART0_TX_PORT                           SL_GPIO_PORT_A
#endif
#ifndef USART0_TX_PIN                           
#define USART0_TX_PIN                            4
#endif

// [USART0]$

// $[I2C1]
// [I2C1]$

// $[EUSART1]
// [EUSART1]$

// $[KEYSCAN]
// [KEYSCAN]$

// $[LETIMER0]
// [LETIMER0]$

// $[IADC0]
// [IADC0]$

// $[ACMP0]
// [ACMP0]$

// $[ACMP1]
// [ACMP1]$

// $[VDAC0]
// [VDAC0]$

// $[VDAC1]
// [VDAC1]$

// $[PCNT0]
// [PCNT0]$

// $[HFXO0]
// [HFXO0]$

// $[I2C0]
// [I2C0]$

// $[EUSART0]
// EUSART0 RX on PA06
#ifndef EUSART0_RX_PORT                         
#define EUSART0_RX_PORT                          SL_GPIO_PORT_A
#endif
#ifndef EUSART0_RX_PIN                          
#define EUSART0_RX_PIN                           6
#endif

// EUSART0 TX on PA05
#ifndef EUSART0_TX_PORT                         
#define EUSART0_TX_PORT                          SL_GPIO_PORT_A
#endif
#ifndef EUSART0_TX_PIN                          
#define EUSART0_TX_PIN                           5
#endif

// [EUSART0]$

// $[PTI]
// PTI DFRAME on PC05
#ifndef PTI_DFRAME_PORT                         
#define PTI_DFRAME_PORT                          SL_GPIO_PORT_C
#endif
#ifndef PTI_DFRAME_PIN                          
#define PTI_DFRAME_PIN                           5
#endif

// PTI DOUT on PC04
#ifndef PTI_DOUT_PORT                           
#define PTI_DOUT_PORT                            SL_GPIO_PORT_C
#endif
#ifndef PTI_DOUT_PIN                            
#define PTI_DOUT_PIN                             4
#endif

// [PTI]$

// $[MODEM]
// [MODEM]$

// $[CUSTOM_PIN_NAME]
#ifndef _PORT                                   
#define _PORT                                    SL_GPIO_PORT_A
#endif
#ifndef _PIN                                    
#define _PIN                                     0
#endif

#ifndef test0_PORT                              
#define test0_PORT                               SL_GPIO_PORT_B
#endif
#ifndef test0_PIN                               
#define test0_PIN                                0
#endif

#ifndef test1_PORT                              
#define test1_PORT                               SL_GPIO_PORT_B
#endif
#ifndef test1_PIN                               
#define test1_PIN                                1
#endif

// [CUSTOM_PIN_NAME]$

#endif // PIN_CONFIG_H

