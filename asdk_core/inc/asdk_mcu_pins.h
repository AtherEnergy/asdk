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
    asdk_adc.h

    @path
    inc/asdk_mcu_pins.h

    @Created on
    Jun 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the ADC DAL module of ASDK.
*/

#ifndef ASDK_PINS_H
#define ASDK_PINS_H

/*!
 @brief An enumeration to represent pin of a microcontroller. The enums are mapped
 with the physical pin numbers of the microcontroller.
*/
typedef enum
{
    MCU_PIN_0 = 0,  /*!< Microcontroller's Pin 0 */
    MCU_PIN_1,      /*!< Microcontroller's Pin 1 */
    
    MCU_PIN_2,      /*!< Microcontroller's Pin 2 */
    MCU_PIN_3,      /*!< Microcontroller's Pin 3 */
    MCU_PIN_4,      /*!< Microcontroller's Pin 4 */
    MCU_PIN_5,      /*!< Microcontroller's Pin 5 */

    MCU_PIN_6,      /*!< Microcontroller's Pin 6 */
    MCU_PIN_7,      /*!< Microcontroller's Pin 7 */
    MCU_PIN_8,      /*!< Microcontroller's Pin 8 */
    MCU_PIN_9,      /*!< Microcontroller's Pin 9 */

    MCU_PIN_10,     /*!< Microcontroller's Pin 10 */
    MCU_PIN_11,     /*!< Microcontroller's Pin 11 */

    MCU_PIN_12,     /*!< Microcontroller's Pin 12 */
    MCU_PIN_13,     /*!< Microcontroller's Pin 13 */

    MCU_PIN_14,     /*!< Microcontroller's Pin 14 */
    MCU_PIN_15,     /*!< Microcontroller's Pin 15 */
    MCU_PIN_16,     /*!< Microcontroller's Pin 16 */
    MCU_PIN_17,     /*!< Microcontroller's Pin 17 */

    MCU_PIN_18,     /*!< Microcontroller's Pin 18 */
    MCU_PIN_19,     /*!< Microcontroller's Pin 19 */
    MCU_PIN_20,     /*!< Microcontroller's Pin 20 */
    MCU_PIN_21,     /*!< Microcontroller's Pin 21 */
    MCU_PIN_22,     /*!< Microcontroller's Pin 22 */
    MCU_PIN_23,     /*!< Microcontroller's Pin 23 */

    MCU_PIN_24,     /*!< Microcontroller's Pin 24 */
    MCU_PIN_25,     /*!< Microcontroller's Pin 25 */
    MCU_PIN_26,     /*!< Microcontroller's Pin 26 */
    MCU_PIN_27,     /*!< Microcontroller's Pin 27 */
    MCU_PIN_28,     /*!< Microcontroller's Pin 28 */

    MCU_PIN_29,     /*!< Microcontroller's Pin 29 */
    MCU_PIN_30,     /*!< Microcontroller's Pin 30 */
    MCU_PIN_31,     /*!< Microcontroller's Pin 31 */
    MCU_PIN_32,     /*!< Microcontroller's Pin 32 */
    MCU_PIN_33,     /*!< Microcontroller's Pin 33 */
    MCU_PIN_34,     /*!< Microcontroller's Pin 34 */

    MCU_PIN_35,     /*!< Microcontroller's Pin 35 */
    MCU_PIN_36,     /*!< Microcontroller's Pin 36 */
    MCU_PIN_37,     /*!< Microcontroller's Pin 37 */

    MCU_PIN_38,     /*!< Microcontroller's Pin 38 */
    MCU_PIN_39,     /*!< Microcontroller's Pin 39 */
    MCU_PIN_40,     /*!< Microcontroller's Pin 40 */

    MCU_PIN_41,     /*!< Microcontroller's Pin 41 */
    MCU_PIN_42,     /*!< Microcontroller's Pin 42 */
    MCU_PIN_43,     /*!< Microcontroller's Pin 43 */
    MCU_PIN_44,     /*!< Microcontroller's Pin 44 */

    MCU_PIN_45,     /*!< Microcontroller's Pin 24 */
    MCU_PIN_46,     /*!< Microcontroller's Pin 25 */
    MCU_PIN_47,     /*!< Microcontroller's Pin 26 */
    MCU_PIN_48,     /*!< Microcontroller's Pin 27 */
    MCU_PIN_49,     /*!< Microcontroller's Pin 28 */

    MCU_PIN_50,     /*!< Microcontroller's Pin 50 */
    MCU_PIN_51,     /*!< Microcontroller's Pin 51 */

    MCU_PIN_52,     /*!< Microcontroller's Pin 52 */
    MCU_PIN_53,     /*!< Microcontroller's Pin 53 */
    MCU_PIN_54,     /*!< Microcontroller's Pin 54 */
    MCU_PIN_55,     /*!< Microcontroller's Pin 55 */
    MCU_PIN_56,     /*!< Microcontroller's Pin 56 */
    MCU_PIN_57,     /*!< Microcontroller's Pin 57 */
    MCU_PIN_58,     /*!< Microcontroller's Pin 58 */
    MCU_PIN_59,     /*!< Microcontroller's Pin 59 */

    MCU_PIN_60,     /*!< Microcontroller's Pin 60 */
    MCU_PIN_61,     /*!< Microcontroller's Pin 61 */
    MCU_PIN_62,     /*!< Microcontroller's Pin 62 */
    MCU_PIN_63,     /*!< Microcontroller's Pin 63 */

    MCU_PIN_64,     /*!< Microcontroller's Pin 64 */
    MCU_PIN_65,     /*!< Microcontroller's Pin 65 */
    MCU_PIN_66,     /*!< Microcontroller's Pin 66 */

    MCU_PIN_67,     /*!< Microcontroller's Pin 67 */
    MCU_PIN_68,     /*!< Microcontroller's Pin 68 */
    MCU_PIN_69,     /*!< Microcontroller's Pin 69 */
    MCU_PIN_70,     /*!< Microcontroller's Pin 70 */
    MCU_PIN_71,     /*!< Microcontroller's Pin 71 */
    MCU_PIN_72,     /*!< Microcontroller's Pin 72 */
    MCU_PIN_73,     /*!< Microcontroller's Pin 73 */
    MCU_PIN_74,     /*!< Microcontroller's Pin 74 */

    MCU_PIN_75,     /*!< Microcontroller's Pin 75 */
    MCU_PIN_76,     /*!< Microcontroller's Pin 76 */

    MCU_PIN_77,     /*!< Microcontroller's Pin 77 */
    MCU_PIN_78,     /*!< Microcontroller's Pin 78 */
    MCU_PIN_79,     /*!< Microcontroller's Pin 79 */
    MCU_PIN_80,     /*!< Microcontroller's Pin 80 */

    MCU_PIN_81,     /*!< Microcontroller's Pin 81 */
    MCU_PIN_82,     /*!< Microcontroller's Pin 82 */
    MCU_PIN_83,     /*!< Microcontroller's Pin 83 */
    MCU_PIN_84,     /*!< Microcontroller's Pin 84 */

    MCU_PIN_85,     /*!< Microcontroller's Pin 85 */
    MCU_PIN_86,     /*!< Microcontroller's Pin 86 */
    MCU_PIN_87,     /*!< Microcontroller's Pin 87 */
    MCU_PIN_88,     /*!< Microcontroller's Pin 88 */
    MCU_PIN_89,     /*!< Microcontroller's Pin 89 */

    MCU_PIN_90,     /*!< Microcontroller's Pin 90 */

    MCU_PIN_91,     /*!< Microcontroller's Pin 91 */
    MCU_PIN_92,     /*!< Microcontroller's Pin 92 */
    MCU_PIN_93,     /*!< Microcontroller's Pin 93 */
    MCU_PIN_94,     /*!< Microcontroller's Pin 94 */

    MCU_PIN_95,     /*!< Microcontroller's Pin 95 */
    MCU_PIN_96,     /*!< Microcontroller's Pin 96 */
    MCU_PIN_97,     /*!< Microcontroller's Pin 97 */
    MCU_PIN_98,     /*!< Microcontroller's Pin 98 */
    MCU_PIN_99,     /*!< Microcontroller's Pin 99 */

    MCU_PIN_MAX,    /*!< Total number of pins is 100 */
    MCU_PIN_NOT_DEFINED = MCU_PIN_MAX,  /*!< Pins beyond @ref MCU_PIN_MAX are not defined. */
} asdk_mcu_pin_t;

#endif /* ASDK_PINS_H */
