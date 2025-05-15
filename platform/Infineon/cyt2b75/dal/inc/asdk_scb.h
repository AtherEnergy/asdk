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
    scb.h

    @path
    platform/Infineon/cyt2b75/dal/inc/asdk_scb.h

    @Created on
    Mar 23, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    Serial Communication Block common APIs.
*/

#ifndef SCB_H
#define SCB_H

// #include "cyt2b75cae.h"
#include "cyt2b75bae.h"
#include <stdint.h>
#include <stdbool.h>

typedef void (*scb_user_cb_type)(void);

typedef enum {
  SCB_INDEX_0 = 0,
  SCB_INDEX_1,
  SCB_INDEX_2,
  SCB_INDEX_3,
  SCB_INDEX_4,
  SCB_INDEX_5,
  SCB_INDEX_6,
  SCB_INDEX_7,
  MAX_SCB_MODULES
} scb_index_t;

// this is default configuration, modify as per your board
#define SCB_MAX_UART_MODULES    8
#define SCB_MAX_I2C_MODULES     8
#define SCB_MAX_SPI_MODULES     8




/* SCB APIs */
void SCB_Set_ISR(uint8_t scb_index, scb_user_cb_type user_callback,uint8_t interrupt_priority);
void SCB_Clear_ISR(uint8_t scb_index);
bool SCB_Availability_Status(volatile stc_SCB_t *base);


#endif /* SCB_H */
