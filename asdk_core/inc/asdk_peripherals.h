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
    asdk_peripherals.h

    @path
    inc/asdk_peripherals.h

    @Created on
    Jul 27, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the PERIPHERALS DAL module of ASDK.
*/

#ifndef ASDK_PERIPHERALS_H
#define ASDK_PERIPHERALS_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

/* sdk includes ****************************** */

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

typedef enum {
    ASDK_GPIO = 0,      // handled by platform dal
    ASDK_ADC,           // handled by platform dal
    ASDK_CAN,           // handled by platform dal
    ASDK_TIMER,         // handled by platform dal
    ASDK_PWM,           // handled by platform dal
    ASDK_UART,          // handled by platform dal
    ASDK_I2C,           // handled by platform dal
    ASDK_SPI,           // handled by platform dal
    ASDK_WDG,           // handled by platform dal
    ASDK_SYSTICK,       // handled by platform dal

    ASDK_PERIPHERAL_MAX,
    ASDK_PERIPHERAL_NOT_DEFINED = ASDK_PERIPHERAL_MAX,
} asdk_peripheral_t;

/*==============================================================================

                   DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/

#endif /* ASDK_PERIPHERALS_H */
