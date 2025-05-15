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
    asdk_clock.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_clock.c

    @Created on
    Jul 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the CLOCK module for CYT2B75 microcontroller.

*/

/* Info from TRM:

Therea are 2 types of clocks in the system as mentioned below. The sources of
these clocks must be configured.

1. High frequency clock (HF)
2. Low frequency clock (LF)

Clock sources:

  1. Internal:
    - Internal Main Oscillator (IMO)         - 8MHz, enabled by default.
    - Internal Low-Speed Oscillator 0 (ILO0) - 32.768KHz, enabled by default.
    - Internal Low-Speed Oscillator 1 (ILO1) - For monitoring ILO0, disabled
                                               by default.
  2. External:
    - External Crystal Oscillator (ECO) - 3.988MHz to 33.34MHz, disabled
                                          by default.
    - Watch Crystal Oscillator (WCO)    - 32.768 kHz for RTC, disabled
                                          by default.
    - External Clock (EXT_CLK)          - 0.25MHz to 80MHz, disabled
                                          by default.

Clock tree:

    1. Select Clock-Sources:
        - HF: IMO, ECO or EXT_CLK.
          - CLK_HF0: Used by core and peripherals so is always enabled.
          - CLK_HF1: Used by event generator (routes interrupts and triggers
                 to GPIO)
          - CLK_HF2: unused (figure 7-1)
        - LF: ECO, WCO or EXT_CLK.

    2. Derive the desired HF clock:
        - Configure CLK_PATH0, CLK_PATH1, CLK_PATH2 or CLK_PATH3 to enable
          CLK_HF0 and CLK_HF1
            - FLL in CLK_PATH0
            - PLL in CLK_PATH1
        - Configure Predivider

    3. Derive clocks for peripherals:
        - CLK_HF0 is the source for:
          - Memories
          - CLK_FAST with div - for CM4 & CPUSS Fast infra
          - CLK_PERI with div is the source for:
            - CLK_SLOW with div - for CM0+ & CPUSS Slow infra
            - CLK_GR3 with div: Timers, PWM, event genrator & HSIOM
            - CLK_GR5 with div: CANFD, LIN
            - CLK_GR6 with div: SCB
            - CLK_GR9 with div: SAR
            - PCLK_X_CLOCK dividers
*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

/* standard includes ************************* */

/* asdk includes ***************************** */

/* dal includes ****************************** */

#include "asdk_clock.h" // dal APIs

/* sdk includes ****************************** */

#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "sysclk/cy_sysclk.h"  // for PLL and clock divider APIs

#ifdef CY_SYSTEM_WDT_DISABLE
#include "syswdt/cy_syswdt.h"
#endif

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define ASDK_IS_DIV_ENABLED(DIV_TYPE, DIV_NUM) ( \
    (DIV_TYPE == CY_SYSCLK_DIV_8_BIT) ? PERI->unDIV_8_CTL[DIV_NUM].stcField.u1EN : PERI->unDIV_16_CTL[DIV_NUM].stcField.u1EN)

#define PLL_ENABLE_TIMEOUT 10000 // from clock config app note (AN220208)

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/

/*!
 * @brief Data strucutre for clock divider. Refer the TRM of the microcontroller for more information.
 *
 * Implements : cyt2b75_div_t
 */
typedef struct
{
    cy_en_divider_types_t type; /*!< Divider type. Refer the TRM of the microcontroller for more information. */
    uint8_t number;             /*!< Divider number. Refer the TRM of the microcontroller for more information. */
    uint32_t integer_value;     /*!< Divider value as integer. Refer the TRM of the microcontroller for more information. */
    uint32_t fractional_value;  /*!< Divider value after the decimal point. Refer the TRM of the microcontroller for more information. */
} cyt2b75_div_t;

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/

static bool asdk_assign_8_16_divider(volatile cyt2b75_div_t *out_div);
static bool asdk_assign_24_5_divider(volatile cyt2b75_div_t *out_div);
static bool asdk_assign_clock_divider(en_clk_dst_t ip_block, uint32_t target_frequency, volatile cyt2b75_div_t *out_div);
static bool asdk_enable_clock_divider(cyt2b75_div_t div);

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

/* volatile global variables ***************** */

/* global variables ************************** */

cy_stc_pll_config_t cyt2b75_pll_cfg = {0};
uint32_t cyt2b75_clock_frequency = 0;

/* static variables ************************** */

static uint32_t peri_clk = 0;
static uint32_t approx_frac_clk = 0;

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/*!This function initializes the system and core-cpu clocks. */
void asdk_clock_init(asdk_clock_config_t *clk_config)
{
    /* calling SystemInit() from system_tviibe1m_<core>.c
       would require the user to modify the clock
       and trim settings in SDK.

       Hence, we create this API to consume user
       configured clock configuration.
    */

#if _CORE_cm0plus_
    cy_stc_pll_config_t *const pll_config = &cyt2b75_pll_cfg;

#if defined(CY_SYSTEM_WDT_DISABLE)
    /* disable WDT */
    Cy_WDT_Disable();
#endif /* CY_SYSTEM_WDT_DISABLE */

    /*********** Setting wait state for ROM **********/
    CPUSS->unROM_CTL.stcField.u2SLOW_WS = 1u;
    CPUSS->unROM_CTL.stcField.u2FAST_WS = 0u;

    /*********** Setting wait state for RAM **********/
    CPUSS->unRAM0_CTL0.stcField.u2SLOW_WS = 1u;
    CPUSS->unRAM0_CTL0.stcField.u2FAST_WS = 0u;

#if defined(CPUSS_RAMC1_PRESENT) && (CPUSS_RAMC1_PRESENT == 1UL)
    CPUSS->unRAM1_CTL0.stcField.u2SLOW_WS = 1u;
    CPUSS->unRAM1_CTL0.stcField.u2FAST_WS = 0u;
#endif /* defined (CPUSS_RAMC1_PRESENT) && (CPUSS_RAMC1_PRESENT == 1UL) */

#if defined(CPUSS_RAMC2_PRESENT) && (CPUSS_RAMC2_PRESENT == 1UL)
    CPUSS->unRAM2_CTL0.stcField.u2SLOW_WS = 1u;
    CPUSS->unRAM2_CTL0.stcField.u2FAST_WS = 0u;
#endif /* defined (CPUSS_RAMC2_PRESENT) && (CPUSS_RAMC2_PRESENT == 1UL) */

    /*********** Setting wait state for FLASH **********/
    FLASHC->unFLASH_CTL.stcField.u4MAIN_WS = 1u;

    /***    Set clock LF source        ***/
    SRSS->unCLK_SELECT.stcField.u3LFCLK_SEL = CY_SYSCLK_LFCLK_IN_ILO0;

    // Select PLL source and apply trim settings
    if (clk_config->clk_source == ASDK_CLOCK_SRC_EXT)
    {
        // PLL source as ECO
        SRSS->unCLK_PATH_SELECT[1 /*PLL0*/].stcField.u3PATH_MUX =
            CY_SYSCLK_CLKPATH_IN_ECO;

        // ECO trim settings
        SRSS->unCLK_ECO_CONFIG2.stcField.u3WDTRIM =
            clk_config->xtal_trim.watchdog_trim;
        SRSS->unCLK_ECO_CONFIG2.stcField.u4ATRIM =
            clk_config->xtal_trim.amplitude_trim;
        SRSS->unCLK_ECO_CONFIG2.stcField.u2FTRIM =
            clk_config->xtal_trim.filter_trim;
        SRSS->unCLK_ECO_CONFIG2.stcField.u2RTRIM =
            clk_config->xtal_trim.feedback_resistor_trim;
        SRSS->unCLK_ECO_CONFIG2.stcField.u3GTRIM =
            clk_config->xtal_trim.gain_trim;

        SRSS->unCLK_ECO_CONFIG.stcField.u1ECO_EN = 1ul;
        while (SRSS->unCLK_ECO_STATUS.stcField.u1ECO_OK == 0ul)
            ;
        while (SRSS->unCLK_ECO_STATUS.stcField.u1ECO_READY == 0ul)
            ;
    }
    else
    {
        // PLL source as IMO
        SRSS->unCLK_PATH_SELECT[1 /*PLL0*/].stcField.u3PATH_MUX =
            CY_SYSCLK_CLKPATH_IN_IMO;

        // ECO trim settings not required for IMO
    }

#if defined(CY_SYSTEM_WCO_ENABLE)

    // Enable WCO
    BACKUP->unCTL.stcField.u1WCO_EN = 1ul;

    // Wait until WCO status becomes OK
    while (BACKUP->unSTATUS.stcField.u1WCO_OK == 0ul)
        ;

#endif

    /***  Set CPUSS dividrs as required        ***/
    // FAST = 160,000,000; PERI and SLOW = FAST / 2;
    CPUSS->unCM4_CLOCK_CTL.stcField.u8FAST_INT_DIV = 0u; // no division
    CPUSS->unCM0_CLOCK_CTL.stcField.u8PERI_INT_DIV = 1u; // divided by 2
    CPUSS->unCM0_CLOCK_CTL.stcField.u8SLOW_INT_DIV = 0u; // no division

    /***     PLL setting and enabling        ***/
    cyt2b75_pll_cfg.inputFreq = clk_config->pll.input_frequency;
    cyt2b75_pll_cfg.outputFreq = clk_config->pll.output_frequency;
    // from clock config app note (AN220208)
    cyt2b75_pll_cfg.outputMode = CY_SYSCLK_FLLPLL_OUTPUT_AUTO;
    cyt2b75_pll_cfg.lfMode = false;

    // use PLL and derive PLL settings from input and output frequency
    Cy_SysClk_PllConfigure(
        CY_SYSCLK_HFCLK_IN_CLKPATH1, // from clock config app note (AN220208)
        pll_config);
    Cy_SysClk_PllEnable(
        CY_SYSCLK_HFCLK_IN_CLKPATH1, // from clock config app note (AN220208)
        PLL_ENABLE_TIMEOUT);         // from clock config app note (AN220208)

    /***   Setting  PATH2  source        ***/
    SRSS->unCLK_PATH_SELECT[2].stcField.u3PATH_MUX = CY_SYSCLK_CLKPATH_IN_IMO;

    /***  Assign  PLL0 as source of clk_hf0        ***/
    /* Select source of clk_hf0 */
    /***  Set HF source, divider, enable   ***/
    SRSS->unCLK_ROOT_SELECT[0 /*clk_hf0*/].stcField.u4ROOT_MUX =
        CY_SYSCLK_HFCLK_IN_CLKPATH1;
    SRSS->unCLK_ROOT_SELECT[0].stcField.u2ROOT_DIV = 0u; /* no div */
    SRSS->unCLK_ROOT_SELECT[0].stcField.u1ENABLE = 1u;   /* 1 = enable */

    /* Select source of clk_hf1 */
    /***  Set HF1 source, divider, enable   ***/
    SRSS->unCLK_ROOT_SELECT[1 /*clk_hf1*/].stcField.u4ROOT_MUX =
        CY_SYSCLK_HFCLK_IN_CLKPATH1;
    SRSS->unCLK_ROOT_SELECT[1].stcField.u2ROOT_DIV = 1u; /* divided by 2 */
    SRSS->unCLK_ROOT_SELECT[1].stcField.u1ENABLE = 1u;   /* 1 = enable */

    /***     Enabling ILO0        ***/
    Cy_WDT_Unlock();
    SRSS->unCLK_ILO0_CONFIG.stcField.u1ENABLE = 1;        /* 1 = enable */
    SRSS->unCLK_ILO0_CONFIG.stcField.u1ILO0_BACKUP = 1ul; /* Ilo HibernateOn */
    Cy_WDT_Lock();
#endif

    // from system_cyt2b7.h and system_tviibe1m_cm0plus.c
    cyt2b75_clock_frequency = clk_config->pll.input_frequency;
    SystemCoreClockUpdate();

    Cy_SysClk_GetClkPeriFrequency(&peri_clk);
    approx_frac_clk = (peri_clk << 5);
}

/*!  This function initializes the peripheral clock. It internally setups the
  clocks tree for the given peripheral and derives the closest value
  for the clock divider. The effective frequency may vary from the desired
  target frequency hence effective_frequency is given as output parameter.*/
asdk_errorcode_t asdk_clock_enable(asdk_clock_peripheral_t *params, double *effective_frequency_hz)
{
    volatile cyt2b75_div_t cy_div = {0};
    asdk_errorcode_t error_status = ASDK_CLOCK_SUCCESS;
    en_clk_dst_t ip_block;
    bool div_assigned = false;
    bool div_enabled = false;
    double divder_value;

    switch (params->peripheral_type)
    {
    case ASDK_ADC:
        // 3 dividers
        // ignore ch_no
        ip_block = PCLK_PASS0_CLOCK_SAR0 + params->module_no;
        break;

    case ASDK_CAN:
        // 6 dividers
        if (0 == params->module_no)
        {
            ip_block = PCLK_CANFD0_CLOCK_CAN0 + params->ch_no;
        }
        else
        {
            ip_block = PCLK_CANFD1_CLOCK_CAN0 + params->ch_no;
        }
        break;

    case ASDK_TIMER:
    case ASDK_PWM:
        // 63 + 16 + 4 dividers
        if (0 == params->module_no)
        {
            ip_block = PCLK_TCPWM0_CLOCKS0 + params->ch_no;
        }
        else if (1 == params->module_no)
        {
            ip_block = PCLK_TCPWM0_CLOCKS256 + params->ch_no;
        }
        else // MCU_MODULE_TIMER_2
        {
            ip_block = PCLK_TCPWM0_CLOCKS512 + params->ch_no;
        }
        break;

    case ASDK_UART:
    case ASDK_I2C:
    case ASDK_SPI:
        // 8 dividers
        // ignore ch_no
        ip_block = PCLK_SCB0_CLOCK + params->module_no;
        break;

    default:
        error_status = ASDK_CLOCK_ERROR_PERIPHERAL_NOT_SUPPORTED;
        break;
    }

    if (ASDK_CLOCK_SUCCESS == error_status)
    {
        div_assigned = asdk_assign_clock_divider(ip_block, params->target_frequency, &cy_div);

        if (div_assigned)
        {
            div_enabled = asdk_enable_clock_divider(cy_div);

            if (div_enabled)
            {
                divder_value = (double)(cy_div.integer_value + 1) + ((double)cy_div.fractional_value / 32.0);

                if (effective_frequency_hz != NULL)
                {
                    *effective_frequency_hz = (double)peri_clk / divder_value;
                }
            }
            else
            {
                error_status = ASDK_CLOCK_ERROR_DIV_NOT_ENABLED;
            }
        }
        else
        {
            error_status = ASDK_CLOCK_ERROR_DIV_NOT_ASSIGNED;
        }
    }

    return error_status;
}

/* static functions ************************** */

static bool asdk_assign_8_16_divider(volatile cyt2b75_div_t *out_div)
{
    bool assigned = false;
    cy_en_sysclk_status_t cy_clk_status = CY_SYSCLK_SUCCESS;

    uint32_t assigned_value;
    uint8_t div_limit = (out_div->type == CY_SYSCLK_DIV_8_BIT) ? PERI_DIV_8_NR : PERI_DIV_16_NR;

    for (uint8_t div_num = 0; div_num < div_limit; div_num++)
    {
        // if already assigned? assign existing divider
        if (ASDK_IS_DIV_ENABLED(out_div->type, div_num))
        {
            assigned_value = Cy_SysClk_PeriphGetDivider(out_div->type, div_num);

            if (assigned_value == out_div->integer_value)
            {
                // assigned
                out_div->number = div_num;
                assigned = true;
                break;
            }
            else
            {
                /* keep looking */
                continue;
            }
        }
        else // else, assign new divider
        {
            cy_clk_status = Cy_SysClk_PeriphSetDivider(out_div->type, div_num, out_div->integer_value);

            if (CY_SYSCLK_SUCCESS == cy_clk_status)
            {
                // assigned
                out_div->number = div_num;
                assigned = true;
            }
            else
            {
                // failed to assign, shouldn't reach here
                assigned = false;
            }

            break;
        }
    }

    return assigned;
}

static bool asdk_assign_24_5_divider(volatile cyt2b75_div_t *out_div)
{
    bool assigned = false;
    cy_en_sysclk_status_t cy_clk_status = CY_SYSCLK_SUCCESS;

    uint32_t div_24_intg_part;
    uint32_t div_5_fract_part;

    for (uint8_t div_num = 0; div_num < PERI_DIV_24_5_NR; div_num++)
    {
        // if already assigned? assign existing divider
        if (PERI->unDIV_24_5_CTL[div_num].stcField.u1EN == 1)
        {
            Cy_SysClk_PeriphGetFracDivider(CY_SYSCLK_DIV_24_5_BIT, div_num, &div_24_intg_part, &div_5_fract_part);

            if ((div_24_intg_part == out_div->integer_value) &&
                (div_5_fract_part == (uint32_t)out_div->fractional_value))
            {
                // assigned
                out_div->number = div_num;
                assigned = true;
                break;
            }
            else
            {
                /* keep looking */
                continue;
            }
        }
        else // else, assign new divider
        {
            cy_clk_status = Cy_SysClk_PeriphSetFracDivider(CY_SYSCLK_DIV_24_5_BIT, div_num, out_div->integer_value, out_div->fractional_value);

            if (CY_SYSCLK_SUCCESS == cy_clk_status)
            {
                // assigned
                out_div->number = div_num;
                assigned = true;
            }
            else
            {
                // failed to assign, shouldn't reach here
                assigned = false;
            }

            break;
        }
    }

    return assigned;
}

static bool asdk_assign_clock_divider(en_clk_dst_t ip_block, uint32_t target_frequency, volatile cyt2b75_div_t *out_div)
{
    // todo: if configured divider with same value already exists then assign the same divider
    // else assign the first available divider
    bool div_assigned = false;
    cy_en_sysclk_status_t cy_clk_status = CY_SYSCLK_SUCCESS;
    double intg_part;
    double frac_part;
    uint32_t intg_part_u32;
    uint32_t frac_divider_value;

    /* determine the best divider type */

    // find if fractional div is required
    double divider_counter = (double)peri_clk / (double)target_frequency;
    frac_part = modf(divider_counter, &intg_part);

    intg_part_u32 = (uint32_t)intg_part & 0xFFFFFF;

    // use 24.5 bit divider if divider value is fractional,
    // also when divider value is > 0xFFFF
    if ((intg_part_u32 > 0xFFFF) || (frac_part > 0.01))
    {
        // assign integer and fraction values
        frac_divider_value = approx_frac_clk / target_frequency;

        out_div->type = CY_SYSCLK_DIV_24_5_BIT;
        out_div->integer_value = ((frac_divider_value & 0x1FFFFFE0) >> 5) - 1;
        out_div->fractional_value = frac_divider_value & 0x1F;

        div_assigned = asdk_assign_24_5_divider(out_div);
    }
    else
    {
        // assign integer value
        out_div->integer_value = intg_part_u32 - 1;
        out_div->fractional_value = 0;
        out_div->type = (intg_part_u32 <= 0xFF) ? CY_SYSCLK_DIV_8_BIT : CY_SYSCLK_DIV_16_BIT;

        div_assigned = asdk_assign_8_16_divider(out_div);
    }

    // map the assigned divider with peripheral
    if (div_assigned)
    {
        cy_clk_status = Cy_SysClk_PeriphAssignDivider(ip_block, out_div->type, out_div->number);

        if (CY_SYSCLK_SUCCESS == cy_clk_status)
        {
            return true;
        }
    }

    return false;
}

static bool asdk_enable_clock_divider(cyt2b75_div_t div)
{
    cy_en_sysclk_status_t cy_clk_status = CY_SYSCLK_SUCCESS;

    cy_clk_status = Cy_SysClk_PeriphEnableDivider(div.type, div.number);

    if (CY_SYSCLK_SUCCESS == cy_clk_status)
    {
        return true;
    }

    return false;
}
