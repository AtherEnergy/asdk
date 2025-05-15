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
    asdk_uart.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_uart.c

    @Created on
    Oct 26, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the UART module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_mcu_pins.h"
#include "asdk_scb.h"
#include "asdk_uart.h"
#include "asdk_pinmux.h"

// sdk includes
#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "scb/cy_scb_uart.h"    // CYT2B75 UART driver APIs
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h" // CYT2B75 system Interrupt APIs

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

#define UART_OVERSAMPLE 8ul

#define E_UART_RX_INTR_FACTER     (                              \
                                 CY_SCB_UART_RX_TRIGGER      |   \
                               /*CY_SCB_UART_RX_NOT_EMPTY    | */\
                               /*CY_SCB_UART_RX_FULL         | */\
                                 CY_SCB_UART_RX_OVERFLOW     |   \
                                 CY_SCB_UART_RX_UNDERFLOW    |   \
                                 CY_SCB_UART_RX_ERR_FRAME    |   \
                                 CY_SCB_UART_RX_ERR_PARITY   |   \
                                 CY_SCB_UART_RX_BREAK_DETECT |   \
                                 0                               \
                                )
#define E_UART_TX_INTR_FACTER     (                              \
                                 CY_SCB_UART_TX_TRIGGER      |   \
                               /*CY_SCB_UART_TX_NOT_FULL     | */\
                               /*CY_SCB_UART_TX_EMPTY        | */\
                                 CY_SCB_UART_TX_OVERFLOW     |   \
                               /*CY_SCB_UART_TX_UNDERFLOW    | */\
                                 CY_SCB_UART_TX_DONE         |   \
                               /*CY_SCB_UART_TX_NACK         | */\
                               /*CY_SCB_UART_TX_ARB_LOST     | */\
                                 0                               \
                                )


/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

extern volatile stc_SCB_t *scb_base_ptrs[];
extern uint8_t scb_uart[];
/* Driver state structure e*/
static cy_stc_scb_uart_context_t g_stc_uart_context[ASDK_UART_MAX];

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
/* static array of function pointer to hold user callback function */
static asdk_uart_callback_fun_t user_uart_callback_fun_list[ASDK_UART_MAX];

/* Callback for all peripherals which supports UART features */
static void UART0_ISR_UserCallback(void);
static void UART1_ISR_UserCallback(void);
static void UART2_ISR_UserCallback(void);
static void UART3_ISR_UserCallback(void);
static void UART4_ISR_UserCallback(void);
static void UART5_ISR_UserCallback(void);
static void UART6_ISR_UserCallback(void);
static void UART7_ISR_UserCallback(void);

scb_user_cb_type UART_User_callbacks[ASDK_UART_MAX] = {
    UART0_ISR_UserCallback,
    UART1_ISR_UserCallback,
    UART2_ISR_UserCallback,
    UART3_ISR_UserCallback,
    UART4_ISR_UserCallback,
    UART5_ISR_UserCallback,
    UART6_ISR_UserCallback,
    UART7_ISR_UserCallback,
};

/* Callback Handlers */
static void scb_uart0_callback_handler(uint32_t event);
static void scb_uart1_callback_handler(uint32_t event);
static void scb_uart2_callback_handler(uint32_t event);
static void scb_uart3_callback_handler(uint32_t event);
static void scb_uart4_callback_handler(uint32_t event);
static void scb_uart5_callback_handler(uint32_t event);
static void scb_uart6_callback_handler(uint32_t event);
static void scb_uart7_callback_handler(uint32_t event);

/* Helping inline functions */
static inline void __asdk_uart_callback_caller(asdk_uart_num_t uart_no, uint32_t event);

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/**
*
* @note 1. Currently UART only supports Standard mode.
* @note 2. Currently UART does not supports RTS and CTS pins
* @todo Add support for CTS and RTS pins
*/
asdk_errorcode_t asdk_uart_init(asdk_uart_config_t *uart_config_data)
{
    asdk_errorcode_t ret_val = ASDK_UART_STATUS_SUCCESS;
    cy_en_scb_uart_status_t cyt_uart_status = CY_SCB_UART_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;
    cy_stc_scb_uart_config_t stc_uart_config = {
        .oversample = UART_OVERSAMPLE,
        .enableMsbFirst = false,
        .enableInputFilter = false,
        .dropOnParityError = false,
        .dropOnFrameError = false,
        .enableMutliProcessorMode = false,
        .receiverAddress = 0,
        .receiverAddressMask = 0,
        .acceptAddrInFifo = false,
        .irdaInvertRx = false,
        .irdaEnableLowPowerReceiver = false,
        .smartCardRetryOnNack = false,
        .enableCts = false,
        .ctsPolarity = CY_SCB_UART_ACTIVE_LOW,
        .rtsRxFifoLevel = 0,
        .rtsPolarity = CY_SCB_UART_ACTIVE_LOW,
        .breakWidth = 0,
        .rxFifoTriggerLevel = 0,
        .rxFifoIntEnableMask = E_UART_RX_INTR_FACTER,
        .txFifoTriggerLevel = 0,
        .txFifoIntEnableMask = E_UART_TX_INTR_FACTER
    };

    /* validate configuration parameters */
    if (NULL == uart_config_data)
    {
        return ASDK_UART_ERROR_NULL_PTR;
    }

    if (ASDK_UART_MAX <= uart_config_data->uart_no)
    {
        return ASDK_UART_ERROR_RANGE_EXCEEDED;
    }

    if (ASDK_UART_BAUD_RATE_1000000 < uart_config_data->baud_rate)
    {
        return ASDK_UART_ERROR_INVALID_BAUDRATE;
    }

    /* clock settings */
    asdk_clock_peripheral_t uart_clock = {
        .module_no = scb_uart[uart_config_data->uart_no],
        .peripheral_type = ASDK_UART,
    };
    
    switch (uart_config_data->op_mode)
    {
    case ASDK_UART_MODE_STANDARD:
        stc_uart_config.uartMode = CY_SCB_UART_STANDARD;
        uart_clock.target_frequency = uart_config_data->baud_rate * UART_OVERSAMPLE;
        break;

    case ASDK_UART_MODE_SMARTCARD:
        return ASDK_UART_ERROR_FEATURE_NOT_IMPLEMENTED;
        break;

    case ASDK_UART_MODE_IRDA:
        return ASDK_UART_ERROR_FEATURE_NOT_IMPLEMENTED;
        break;
    
    default:
        break;
    }

    ret_val = asdk_clock_enable(&uart_clock, NULL);
    if (ASDK_CLOCK_SUCCESS != ret_val)
    {
        return ret_val;
    }

    /* pinmux settings */
    asdk_pinmux_config_t UART_pinmux_array[2] = {
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_UART, ASDK_PINMUX_UART_SUBFUN_TX, scb_uart[uart_config_data->uart_no]), .MCU_pin_num = uart_config_data->uart_tx_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_STRONG_IN_OFF},
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_UART, ASDK_PINMUX_UART_SUBFUN_RX, scb_uart[uart_config_data->uart_no]), .MCU_pin_num = uart_config_data->uart_rx_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_HIGHZ},
        // todo: Add CTS and RTS pin config
    };

    ret_val = asdk_set_pinmux(UART_pinmux_array, 2);
    if (ASDK_PINMUX_SUCCESS != ret_val)
    {
        return ret_val;
    }

    /* Cyt specific :Mapping UART no. to the associated SCB Module */
    scb_index = scb_uart[uart_config_data->uart_no];
    scb = scb_base_ptrs[scb_index];

    if (false == SCB_Availability_Status(scb))
    {
        return ASDK_UART_ERROR_MODULE_UNAVAILABLE;
    }

    /* If interrupt enabled, configure it as per priority */
    if (true == uart_config_data->interrupt_config.use_interrupt)
    {
        SCB_Set_ISR(scb_index, UART_User_callbacks[uart_config_data->uart_no], uart_config_data->interrupt_config.priority);
    }
    else
    {
        return ASDK_UART_ERROR_ISR_REQUIRED;
    }

    /* Updating necessary configurations */
    if (ASDK_UART_DATA_BITS_UNDEFINED <= uart_config_data->data_bits)
        return ASDK_UART_ERROR_INVALID_DATABITS;
    else
        stc_uart_config.dataWidth = uart_config_data->data_bits;

    if (ASDK_UART_PARITY_MAX <= uart_config_data->parity_mode)
        return ASDK_UART_ERROR_INVALID_PARITY;
    else
        stc_uart_config.parity = uart_config_data->parity_mode;

    if (ASDK_UART_STOP_BITS_MAX <= uart_config_data->stop_bits)
        return ASDK_UART_ERROR_INVALID_STOPBITS;
    else
        stc_uart_config.stopBits = uart_config_data->stop_bits == ASDK_UART_STOP_BITS_1 ? CY_SCB_UART_STOP_BITS_1 : CY_SCB_UART_STOP_BITS_2;

    /* De-Initialize the UART Instance */
    Cy_SCB_UART_DeInit(scb);

    /* SCB Initilization for UART */
    cyt_uart_status = Cy_SCB_UART_Init(scb, &stc_uart_config, &g_stc_uart_context[uart_config_data->uart_no]);
    if (CY_SCB_UART_SUCCESS == cyt_uart_status)
    {
        Cy_SCB_UART_Enable(scb);
        ret_val = ASDK_UART_STATUS_SUCCESS;
    }
    else
    {
        /*Return Error UART Init failed*/
        ret_val = ASDK_UART_ERROR_INIT_FAIL;
    }

    return ret_val;
}

asdk_errorcode_t asdk_uart_deinit(asdk_uart_num_t uart_no)
{
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;

    if (ASDK_UART_MAX <= uart_no)
    {
        return ASDK_UART_ERROR_RANGE_EXCEEDED;
    }

    /* Cyt specific: Mapping UART no. to the associated SCB Module */
    scb_index = scb_uart[uart_no];
    scb = scb_base_ptrs[scb_index];

    if (0 == scb->unCTRL.stcField.u1ENABLED)
    {
        return ASDK_UART_ERROR_NOT_INITIALIZED;
    }

    /* De-Initialize the UART Instance */
    Cy_SCB_UART_Disable(scb, &g_stc_uart_context[uart_no]);
    Cy_SCB_UART_DeInit(scb);

    return ASDK_UART_STATUS_SUCCESS;
}

asdk_errorcode_t asdk_uart_install_callback(asdk_uart_num_t uart_no, asdk_uart_callback_fun_t callback_fun)
{
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

    /* Check for Max UART module */
    if (ASDK_UART_MAX <= uart_no)
    {
        return ASDK_UART_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_uart[uart_no];
    scb = scb_base_ptrs[scb_index];

    /* store callback function */
    user_uart_callback_fun_list[uart_no] = callback_fun;

    /* assign hw callback function name */
    switch (uart_no)
    {
    case 0:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart0_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 1:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart1_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 2:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart2_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 3:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart3_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 4:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart4_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 5:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart5_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 6:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart6_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    case 7:
        Cy_SCB_UART_RegisterCallback(scb, (scb_uart_handle_events_t)scb_uart7_callback_handler, &g_stc_uart_context[uart_no]);
        break;
    
    default:
        break;
    }

    return ASDK_UART_STATUS_SUCCESS;
}

asdk_errorcode_t asdk_uart_write_non_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len)
{
    cy_en_scb_uart_status_t cy_uart_status = CY_SCB_UART_SUCCESS;
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

    /* check for max uart module */
    if (ASDK_UART_MAX <= uart_no)
    {
        return ASDK_UART_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_uart[uart_no];
    scb = scb_base_ptrs[scb_index];

    /* UART Transmit */
    cy_uart_status = Cy_SCB_UART_Transmit(scb, data, data_len, &g_stc_uart_context[uart_no]);
    if (CY_SCB_UART_SUCCESS != cy_uart_status)
    {
        return ASDK_UART_ERROR_WRITE_FAIL;
    }

    return ASDK_UART_STATUS_SUCCESS;
}

asdk_errorcode_t asdk_uart_read_non_blocking(asdk_uart_num_t uart_no, uint8_t *data, uint32_t data_len)
{
    cy_en_scb_uart_status_t cy_uart_status = CY_SCB_UART_SUCCESS;
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

    /* check for max uart module */
    if (ASDK_UART_MAX <= uart_no)
    {
        return ASDK_UART_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_uart[uart_no];
    scb = scb_base_ptrs[scb_index];

    /* UART Receive */
    cy_uart_status = Cy_SCB_UART_Receive(scb, data, data_len, &g_stc_uart_context[uart_no]);
    if (CY_SCB_UART_SUCCESS != cy_uart_status)
    {
        return ASDK_UART_ERROR_READ_FAIL;
    }   

    return ASDK_UART_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Function : __asdk_uart_callback_caller */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This inline function calls the respective UART callback function w.r.t. ASDK UART number 
  and passes the ASDK_UART_EVENT.

  @param asdk_uart_num_t uart_no - ASDK UART nnumber
  @param uint32_t event - SCB UART Event type

  @return void
*/
void __asdk_uart_callback_caller(asdk_uart_num_t uart_no, uint32_t event)
{
    switch (event)
    {
    case CY_SCB_UART_TRANSMIT_DONE_EVENT:
        {
            user_uart_callback_fun_list[uart_no](uart_no, g_stc_uart_context[uart_no].txBuf, g_stc_uart_context[uart_no].txBufSize, ASDK_UART_STATUS_TRANSMIT_COMPLETE);
        }
        break;

    case CY_SCB_UART_RECEIVE_DONE_EVENT:
        {
            user_uart_callback_fun_list[uart_no](uart_no, g_stc_uart_context[uart_no].rxBuf, g_stc_uart_context[uart_no].rxBufSize, ASDK_UART_STATUS_RECEIVE_COMPLETE);
        }
        break;

    case CY_SCB_UART_RB_FULL_EVENT:
        //todo: implement ring buffer first, then handle event
        break;

    case CY_SCB_UART_TRANSMIT_ERR_EVENT:
        {
            user_uart_callback_fun_list[uart_no](uart_no, NULL, 0, ASDK_UART_STATUS_TRANSMIT_ERROR);
        }
        break;
    
    case CY_SCB_UART_RECEIVE_ERR_EVENT:
        {
            if (CY_SCB_UART_RECEIVE_BREAK_DETECT & g_stc_uart_context[uart_no].rxStatus)
            {
                user_uart_callback_fun_list[uart_no](uart_no, NULL, 0, ASDK_UART_STATUS_RECEIVE_BREAK_DETECT);
            }
            else if (CY_SCB_UART_RECEIVE_ERR_PARITY & g_stc_uart_context[uart_no].rxStatus)
            {
                user_uart_callback_fun_list[uart_no](uart_no, NULL, 0, ASDK_UART_STATUS_RECEIVE_ERR_PARITY);
            }
            else if (CY_SCB_UART_RECEIVE_ERR_FRAME & g_stc_uart_context[uart_no].rxStatus)
            {
                user_uart_callback_fun_list[uart_no](uart_no, NULL, 0, ASDK_UART_STATUS_RECEIVE_ERR_FRAME);
            }
            else if (CY_SCB_UART_RECEIVE_OVERFLOW & g_stc_uart_context[uart_no].rxStatus)
            {
                user_uart_callback_fun_list[uart_no](uart_no, NULL, 0, ASDK_UART_STATUS_RECEIVE_OVERFLOW);
            }
        }
        
    default:
        break;
    }
}

/*----------------------------------------------------------------------------*/
/* Function : scb_<uart_no>_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callback handlers for respective UART instances

  @param uint32_t event - SCB UART Event type

  @return void

*/
void scb_uart0_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(0, event);
}

void scb_uart1_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(1, event);
}

void scb_uart2_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(2, event);
}

void scb_uart3_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(3, event);
}

void scb_uart4_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(4, event);
}

void scb_uart5_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(5, event);
}

void scb_uart6_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(6, event);
}

void scb_uart7_callback_handler(uint32_t event)
{
    /* Execute UART callback */
    __asdk_uart_callback_caller(7, event);
}

/*----------------------------------------------------------------------------*/
/* Function : UART<uart_no>_ISR_UserCallback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  Callbacks for respective UART instances

  @param void 

  @return void

*/
void UART0_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[0]], &g_stc_uart_context[0]);
}

void UART1_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[1]], &g_stc_uart_context[1]);
}

void UART2_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[2]], &g_stc_uart_context[2]);
}

void UART3_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[3]], &g_stc_uart_context[3]);
}

void UART4_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[4]], &g_stc_uart_context[4]);
}

void UART5_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[5]], &g_stc_uart_context[5]);
}

void UART6_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[6]], &g_stc_uart_context[6]);
}

void UART7_ISR_UserCallback(void)
{
    /* UART interrupt handler for High-Level APIs */
    Cy_SCB_UART_Interrupt(scb_base_ptrs[scb_uart[7]], &g_stc_uart_context[7]);
}
