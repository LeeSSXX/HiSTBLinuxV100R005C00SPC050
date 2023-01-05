#ifndef HI3798MX_TIMER_H
#define HI3798MX_TIMER_H

#define TIMER8_BASE     0xF8A81000    //time8 for secure os
#define TIMER9_BASE     0xF8A81020   //timer9 for secure os(Do not power off)

#define TIMER01_BASE     TIMER8_BASE    /* for adapt platform/tzarch/armv7/tzinit/sre_reset_vector.S */

#define TIMER89_IRQ     (60)

#define TIMER_CLK_RATE  (24 * 1000 * 1000)  /* 24MHZ */

#define FREE_RUNNING_TIMER_BASE     TIMER8_BASE
#define TICK_TIMER_BASE             TIMER9_BASE

#define FREE_RUNNING_TIMER_NUM    (0)
#define TICK_TIMER_NUM (0)


#define FREE_RUNNING_FIQ_NUMBLER     TIMER89_IRQ
#define TICK_TIMER_FIQ_NUMBLER       TIMER89_IRQ

/* SPI number to call normal world tzdriver/tc_client_driver.c tc_secure_notify */
#define SPI_NUM_FOR_NOTIFY (93)   /* TODO: 61+32 */

#define TIMER_LOAD      0x00
#define TIMER_VALUE     0x04
#define TIMER_CTRL      0x08

#define TIMER_LOAD_HIGH      0x20
#define TIMER_VALUE_HIGH     0x24
#define TIMER_BGLOAD_HIGH    0x38

#define TIMER_CTRL_ONESHOT  (1 << 0)
#define TIMER_CTRL_32BIT    (1 << 1)
#define TIMER_CTRL_DIV1     (0 << 2)
#define TIMER_CTRL_DIV16    (1 << 2)
#define TIMER_CTRL_DIV256   (2 << 2)
#define TIMER_CTRL_IE       (1 << 5)    /* Interrupt Enable (versatile only) */
#define TIMER_CTRL_PERIODIC (1 << 6)
#define TIMER_CTRL_ENABLE   (1 << 7)

#define TIMER_GT_CLK_TIMER1  (1 << 3)
#define TIMER_GT_PCLK_TIMER1 (1 << 2)
#define TIMER_GT_PCLK_TIMER6 (1 << 24)


#define TIMER_INTCLR        0x0c
#define TIMER_RIS       0x10
#define TIMER_MIS       0x14
#define TIMER_BGLOAD        0x18

#endif /* HI3798MX_TIMER_H */