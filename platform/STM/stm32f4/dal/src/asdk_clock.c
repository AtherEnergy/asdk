/*
Disclaimer:

    The information/code provided by Ather Energy Private Limited (“Ather”) in
    this file is for informational purposes only. All information/code in this
    file is provided in good faith.  However, Ather makes no representation or
    warranty of any kind, express or implied, regarding the accuracy, adequacy,
    validity, reliability, availability or completeness of any information/code
    in the file. Under no circumstances shall Ather have any liability to you
    for any loss or damage of any kind incurred as a result of reliance on any
    information/code provided in the file. Your reliance on any Ather’s
    information/code in the file is solely at your own risk.

    @file
    asdk_clock.c

    @path
    platform/stm32/f4/dal/src/asdk_clock.c

    @Created on
    Jul 26, 2023

    @Author
    ajmeri.j

    @Copyright
    Copyright (c) Ather Energy Pvt Ltd.  All rights reserved.

    @brief
    This file implements the CLOCK module for STM32F4 microcontroller.
    This module was implemented with respect to STM32F407 MCU which is
    present in the STM32F4DISC1 Discovery Kit. So, relevance of the
    configurations in this module needs to be verified before using with
    other MCUs in the STM32F4 family.
*/

/* Info from TRM:

There are 2 types of clocks in the system as mentioned below. The sources of
these clocks must be configured.

1. High Speed External (HSE)
2. High Speed Internal (HSI)

Clock sources:

  1. Internal:
    - Internal High Speed RC Oscillator (HSI) - 16MHz, enabled by default.

  2. External:
    - External High Speed Crystal Oscillator (HSE) - 4 MHz to
      26 MHz, disabled by default


Clock tree:

    1. Select Clock-Sources for PLL:
        - HSE or HSI. HSI is the default source for the PLL.

    2. Derive the desired HCLK clock:
        - PLL output = Input Frequency * N / (M * P)
        - Choose SYSCLK from HSI, HSE or PLL output.
        - HCLK(System Core Clock) = SYSCLK / HPRE.

    3. Derive clocks for peripherals:
        - HCLK is the source for:
          - Ethernet PTP clock
          - AHB bus
          - Systick Timer with divider of 1
          - APB1 peripherals with a divider of 4
          - APB1 timer with a divider of 4 and multiplier of 2
          - APB2 peripherals with a divider of 2
          - APB2 timer with a divider of 2 and multiplier of 2
          - 48 MHz clock = Input Frequency * N / (M * Q)


    NOTE: The PLL multipliers and prescalers are calculated for the given
    set of input and output frequencies meeting frequency ranges of each stage
    of the PLL.
*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

#include "asdk_clock.h" // dal APIs

/* sdk includes ****************************** */

#include "asdk_clock_values.h"
#include "asdk_platform.h"

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

typedef struct {
  asdk_hse_crystal_freq_t input_freq_mhz;
  asdk_output_clock_freq_t output_freq_mhz;
  uint8_t pll_m;
  uint16_t pll_n;
  uint8_t pll_p;
  uint8_t pll_q;
  uint16_t hpre;
} asdk_pll_config_t;

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

/* static variables ************************** */

static asdk_pll_config_t pll_config_lut[] = {
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_4MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 2, 168, 2,
     7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_5MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 5, 336, 2,
     7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_6MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 3, 168, 2,
     7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_8MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 4, 168, 2,
     7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_10MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 5, 168,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_12MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 6, 168,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_16MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 8, 168,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_20MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 10, 168,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_24MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 12, 168,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_25MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 25, 336,
     2, 7, 1},
    {ASDK_CLOCK_HSE_CRYSTAL_FREQ_26MHZ, ASDK_CLOCK_OUTPUT_FREQ_168MHZ, 13, 168,
     2, 7, 1},
};

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/*!This function initializes the system and core-cpu clocks. */
void asdk_clock_init(asdk_clock_config_t *clk_config) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device
     is clocked below the maximum system frequency, to update the voltage
     scaling value regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  if (clk_config->clk_source == ASDK_CLOCK_SRC_INT) {
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSEState = RCC_HSE_OFF;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;

    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 168;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* AHB clock divider will be 1 for input freq = 16MHz, output freq = 168MHz
     */
    RCC_ClkInitStruct.AHBCLKDivider = RCC_CFGR_HPRE_DIV1;
  } else {
    uint8_t i = 0;

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

    for (i = 0; i < sizeof(pll_config_lut) / sizeof(asdk_pll_config_t); i++) {
      if (pll_config_lut[i].input_freq_mhz == clk_config->pll.input_frequency) {
        RCC_OscInitStruct.PLL.PLLM = pll_config_lut[i].pll_m;
        RCC_OscInitStruct.PLL.PLLN = pll_config_lut[i].pll_n;
        RCC_OscInitStruct.PLL.PLLP = pll_config_lut[i].pll_p;
        RCC_OscInitStruct.PLL.PLLQ = pll_config_lut[i].pll_q;
        break;
      }
    }

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select the AHB clock divider based on input and output clk frequency */
    switch (pll_config_lut[i].hpre) {
    case 1:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
      break;

    case 2:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
      break;

    case 4:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV4;
      break;

    case 8:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV8;
      break;

    case 16:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV16;
      break;

    case 64:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV64;
      break;

    case 128:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV128;
      break;

    case 256:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV256;
      break;

    case 512:
      RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV512;
      break;

    default:
      break;
    }
  }

  /** Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  /**
   *  NOTE: Flash Latency varies for the SYSCLK frequency values and the MCU
   * chosen Refer datasheet of the specific MCU for the wait states associated
   * with RCC->CFGR register
   * */
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

  SystemCoreClockUpdate();
}

/*!  This function initializes the peripheral clock. It internally setups the
  clocks tree for the given peripheral and derives the closest value
  for the clock divider. The effective frequency may vary from the desired
  target frequency hence effective_frequency is given as output parameter.*/
asdk_errorcode_t asdk_clock_enable(asdk_clock_peripheral_t *params,
                                   double *effective_frequency_hz) {

  asdk_errorcode_t error_status = ASDK_CLOCK_SUCCESS;
  *effective_frequency_hz = 0;

  switch (params->peripheral_type) {
  case ASDK_ADC:
    switch (params->module_no) {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 2:
      RCC->APB2ENR |= RCC_APB2ENR_ADC2EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 3:
      RCC->APB2ENR |= RCC_APB2ENR_ADC3EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    default:
      break;
    }
    break;

  case ASDK_CAN:
    switch (params->module_no) {
    case 1:
      RCC->APB1ENR |= RCC_APB1ENR_CAN1EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_CAN2EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    default:
      break;
    }
    break;

  case ASDK_TIMER:
  case ASDK_PWM:
    switch (params->module_no) {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 4:
      RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 5:
      RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 6:
      RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 7:
      RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 8:
      RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 9:
      RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 10:
      RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 11:
      RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 12:
      RCC->APB1ENR |= RCC_APB1ENR_TIM12EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 13:
      RCC->APB1ENR |= RCC_APB1ENR_TIM13EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 14:
      RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    default:
      break;
    }
    break;

  case ASDK_UART:
    switch (params->module_no) {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 4:
      RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 5:
      RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 6:
      RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    default:
      break;
    }
    break;

  case ASDK_I2C:
    switch (params->module_no) {
    case 1:
      RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    default:
      break;
    }
    break;

  case ASDK_SPI:
    switch (params->module_no) {
    case 1:
      RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
      *effective_frequency_hz = SystemCoreClock;
      break;

    case 2:
      RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    case 3:
      RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
      *effective_frequency_hz = SystemCoreClock / 2;
      break;

    default:
      break;
    }
    break;

  default:
    error_status = ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED;
    break;
  }

  return error_status;
}

/* static functions ************************** */
