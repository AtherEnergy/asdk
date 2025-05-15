/*
Disclaimer:

    The information/code provided by Ather Energy Limited (“Ather”) in
    this file is for informational purposes only. All information/code in this
    file is provided in good faith.  However, Ather makes no representation or
    warranty of any kind, express or implied, regarding the accuracy, adequacy,
    validity, reliability, availability or completeness of any information/code
    in the file. Under no circumstances shall Ather have any liability to you
    for any loss or damage of any kind incurred as a result of reliance on any
    information/code provided in the file. Your reliance on any Ather’s
    information/code in the file is solely at your own risk.

    @file
    asdk_scb.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_scb.c

    @Created on
    Mar 23, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    Serial Communication Block common APIs for CYT2B75 microcontroller.
*/

/* DAL includes */
#include "asdk_scb.h"

/* SDL includes */
#include "sysint/cy_sysint.h"
#include "scb/cy_scb_i2c.h"
#include "scb/cy_scb_uart.h"
#include "scb/cy_scb_spi.h"


/* SCB interrupt handlers */
void scb0_isr(void);
void scb1_isr(void);
void scb2_isr(void);
void scb3_isr(void);
void scb4_isr(void);
void scb5_isr(void);
void scb6_isr(void);
void scb7_isr(void);

/* SCB base ptrs */
volatile stc_SCB_t *scb_base_ptrs[MAX_SCB_MODULES] = {
    SCB0, 
    SCB1, 
    SCB2, 
    SCB3,
    SCB4,
    SCB5,
    SCB6,
    SCB7
};

// this is default configuration, modify as per your board
uint8_t scb_uart[SCB_MAX_UART_MODULES] = {SCB_INDEX_0, SCB_INDEX_1, SCB_INDEX_2, SCB_INDEX_3, SCB_INDEX_4, SCB_INDEX_5, SCB_INDEX_6, SCB_INDEX_7};
uint8_t scb_i2c[SCB_MAX_I2C_MODULES] =   {SCB_INDEX_0, SCB_INDEX_1, SCB_INDEX_2, SCB_INDEX_3, SCB_INDEX_4, SCB_INDEX_5, SCB_INDEX_6, SCB_INDEX_7};
uint8_t scb_spi[SCB_MAX_SPI_MODULES] =   {SCB_INDEX_0, SCB_INDEX_1, SCB_INDEX_2, SCB_INDEX_3, SCB_INDEX_4, SCB_INDEX_5, SCB_INDEX_6, SCB_INDEX_7};
/* IRQ numbers for corresponding SCB blocks */
cy_en_intr_t scb_isr_irqn[MAX_SCB_MODULES] = {
  scb_0_interrupt_IRQn,
  scb_1_interrupt_IRQn,
  scb_2_interrupt_IRQn,
  scb_3_interrupt_IRQn,
  scb_4_interrupt_IRQn,
  scb_5_interrupt_IRQn,
  scb_6_interrupt_IRQn,
  scb_7_interrupt_IRQn
};

/* Mapping between SCB block and Interrupt handler */
cy_systemIntr_Handler scb_isr_handlers[MAX_SCB_MODULES] = {
    scb0_isr,
    scb1_isr,
    scb2_isr,
    scb3_isr,
    scb4_isr,
    scb5_isr,
    scb6_isr,
    scb7_isr
};

/* Mapping between SCB block and user callback */
scb_user_cb_type scb_user_callback[MAX_SCB_MODULES] = {0};

/* SCB IRQ handlers */
void scb0_isr(void)
{
    if (scb_user_callback[SCB_INDEX_0] != NULL)
        scb_user_callback[SCB_INDEX_0]();
}

void scb1_isr(void)
{
    if (scb_user_callback[SCB_INDEX_1] != NULL)
        scb_user_callback[SCB_INDEX_1]();
}

void scb2_isr(void)
{
    if (scb_user_callback[SCB_INDEX_2] != NULL)
        scb_user_callback[SCB_INDEX_2]();
}

void scb3_isr(void)
{
    if (scb_user_callback[SCB_INDEX_3] != NULL)
        scb_user_callback[SCB_INDEX_3]();
}

void scb4_isr(void)
{
    if (scb_user_callback[SCB_INDEX_4] != NULL)
        scb_user_callback[SCB_INDEX_4]();
}

void scb5_isr(void)
{
    if (scb_user_callback[SCB_INDEX_5] != NULL)
        scb_user_callback[SCB_INDEX_5]();
}

void scb6_isr(void)
{
    if (scb_user_callback[SCB_INDEX_6] != NULL)
        scb_user_callback[SCB_INDEX_6]();
}

void scb7_isr(void)
{
    if (scb_user_callback[SCB_INDEX_7] != NULL)
        scb_user_callback[SCB_INDEX_7]();
}

/* SCB APIs */
void SCB_Set_ISR(uint8_t scb_index, scb_user_cb_type user_callback, uint8_t interrupt_priority)
{
    // todo: index check
    cy_stc_sysint_irq_t irq_cfg = {
        .sysIntSrc  = scb_isr_irqn[scb_index],
        .intIdx     = CPUIntIdx3_IRQn,
        .isEnabled  = true,
    };

    // todo: directly assing ISR from parameter
    if (user_callback != NULL)
        scb_user_callback[scb_index] = user_callback;

    Cy_SysInt_InitIRQ(&irq_cfg);
    Cy_SysInt_SetSystemIrqVector(irq_cfg.sysIntSrc, scb_isr_handlers[scb_index]);
    NVIC_SetPriority(irq_cfg.intIdx, interrupt_priority);
    NVIC_EnableIRQ(irq_cfg.intIdx);
}

void SCB_Clear_ISR(uint8_t scb_index)
{
    // todo: index check
    scb_user_callback[scb_index] = NULL;
}

bool SCB_Availability_Status(volatile stc_SCB_t *scb)
{
    if ((scb->unCTRL.u32Register) & (0x80000000))
        return false;
    else
        return true; 
}
