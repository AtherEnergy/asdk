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
    asdk_i2c.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_i2c.c

    @Created on
    Sept 9, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the I2C module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_mcu_pins.h"
#include "asdk_scb.h"
#include "asdk_i2c.h"
#include "asdk_pinmux.h"

// sdk includes
#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "scb/cy_scb_i2c.h"    // CYT2B75 I2C driver APIs
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h" // CYT2B75 system Interrupt APIs

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/

extern volatile stc_SCB_t *scb_base_ptrs[];
extern uint8_t scb_i2c[];
/* Driver state structur e*/
static cy_stc_scb_i2c_context_t g_stc_i2c_context[ASDK_I2C_MAX];
static cy_stc_scb_i2c_master_xfer_config_t g_stc_i2c_master_tfr_cfg[ASDK_I2C_MAX];

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
/* static array of function pointer to hold user callback function */
static asdk_i2c_callback_fun_t user_i2c_callback_fun_list[ASDK_I2C_MAX];

/* Callback for all peripherals which supports I2C features */
static void I2C0_ISR_UserCallback(void);
static void I2C1_ISR_UserCallback(void);
static void I2C2_ISR_UserCallback(void);
static void I2C3_ISR_UserCallback(void);
static void I2C4_ISR_UserCallback(void);
static void I2C5_ISR_UserCallback(void);
static void I2C6_ISR_UserCallback(void);
static void I2C7_ISR_UserCallback(void);

scb_user_cb_type I2C_User_callbacks[ASDK_I2C_MAX] = {
    I2C0_ISR_UserCallback,
    I2C1_ISR_UserCallback,
    I2C2_ISR_UserCallback,
    I2C3_ISR_UserCallback,
    I2C4_ISR_UserCallback,
    I2C5_ISR_UserCallback,
    I2C6_ISR_UserCallback,
    I2C7_ISR_UserCallback,
};

/* Callback handlers */
static void scb_i2c0_callback_handler(uint32_t event);
static void scb_i2c1_callback_handler(uint32_t event);
static void scb_i2c2_callback_handler(uint32_t event);
static void scb_i2c3_callback_handler(uint32_t event);
static void scb_i2c4_callback_handler(uint32_t event);
static void scb_i2c5_callback_handler(uint32_t event);
static void scb_i2c6_callback_handler(uint32_t event);
static void scb_i2c7_callback_handler(uint32_t event);

/* Helping inline functions */
static inline asdk_i2c_status_t __asdk_get_i2c_status(asdk_i2c_num_t i2c_no, uint32_t event);

/*==============================================================================

                            LOCAL FUNCTION DEFINITIONS

==============================================================================*/

/**
* @todo Implement polling based.
*/
asdk_errorcode_t asdk_i2c_init(asdk_i2c_config_t *i2c_config_data)
{
    cy_stc_scb_i2c_config_t scb_i2c_cfg;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;
    cy_en_scb_i2c_status_t cyt_i2c_status = CY_SCB_I2C_SUCCESS;

    /* clock settings */
    asdk_clock_peripheral_t i2c_clock = {
        .module_no = scb_i2c[i2c_config_data->i2c_no],
        .peripheral_type = ASDK_I2C,
    };

    switch (i2c_config_data->transfer_rate)
    {
    case ASDK_I2C_TRANSFER_RATE_100kbps:
        i2c_clock.target_frequency = 2000000ul;
        break;

    case ASDK_I2C_TRANSFER_RATE_400kbps:
        i2c_clock.target_frequency = 10000000ul;
        break;

    case ASDK_I2C_TRANSFER_RATE_1mbps:
        i2c_clock.target_frequency = 16000000ul;
        break;
        
    default:
        return ASDK_I2C_ERROR_INVALID_TRANSFER_RATE;
        break;
    }

    /* validate already initialized */
    if (CY_SCB_I2C_IDLE == g_stc_i2c_context[i2c_config_data->i2c_no].state)
    {
        return ASDK_I2C_ERROR_INITIALIZED;
    }

    ret_val = asdk_clock_enable(&i2c_clock, NULL);
    if (ASDK_CLOCK_SUCCESS != ret_val)
    {
        return ret_val;
    }

    /* pin mux settings */
    asdk_pinmux_config_t I2C_pinmux_array[2] = {
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C, ASDK_PINMUX_I2C_SUBFUN_SDA, scb_i2c[i2c_config_data->i2c_no]), .MCU_pin_num = i2c_config_data->i2c_sda_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW},
        {.alternate_fun_id = ASDK_PINMUX_ALTFUN(ASDK_PINMUX_FUN_I2C, ASDK_PINMUX_I2C_SUBFUN_SCL, scb_i2c[i2c_config_data->i2c_no]), .MCU_pin_num = i2c_config_data->i2c_scl_mcu_pin_no, .pull_configuration = ASDK_GPIO_PULL_TYPE_OD_DRIVESLOW},
    };

    ret_val = asdk_set_pinmux(I2C_pinmux_array, 2);
    if (ASDK_PINMUX_SUCCESS != ret_val)
    {
        return ret_val;
    }

    /* validate configuration parameters */
    if (NULL == i2c_config_data)
    {
        return ASDK_I2C_ERROR_NULL_PTR;
    }

    if (ASDK_I2C_MAX <= i2c_config_data->i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    if (ASDK_I2C_TRANSFER_RATE_MAX <= i2c_config_data->transfer_rate)
    {
        return ASDK_I2C_ERROR_INVALID_TRANSFER_RATE;
    }

    /*Cyt specific :Mapping I2C no. to the associated SCB Module*/
    scb_index = scb_i2c[i2c_config_data->i2c_no];
    scb = scb_base_ptrs[scb_index];

    if (false == SCB_Availability_Status(scb))
    {
        return ASDK_I2C_ERROR_MODULE_UNAVAILABLE;
    }

    /* If interrupt not disabled, configure the user interrupt handler as per priority */
    if (true == i2c_config_data->interrupt_config.use_interrupt)
    {
        SCB_Set_ISR(scb_index, I2C_User_callbacks[i2c_config_data->i2c_no], i2c_config_data->interrupt_config.priority);
    }
    else
    {
        return ASDK_I2C_ERROR_ISR_REQUIRED;
    }

    /*Updating the necessary configuration as per the Master/Slave type*/
    switch (i2c_config_data->i2c_mode)
    {
        case ASDK_I2C_MODE_MASTER:
            scb_i2c_cfg.i2cMode = CY_SCB_I2C_MASTER;
            scb_i2c_cfg.slaveAddress = 0;
            scb_i2c_cfg.slaveAddressMask = 0;
            break;
        
        case ASDK_I2C_MODE_SLAVE:
            scb_i2c_cfg.i2cMode = CY_SCB_I2C_SLAVE;
            scb_i2c_cfg.slaveAddress = i2c_config_data->slave_Address;
            scb_i2c_cfg.slaveAddressMask = i2c_config_data->slave_Address_Mask;
            break;

        case ASDK_I2C_MODE_MASTER_SLAVE:
            scb_i2c_cfg.i2cMode = CY_SCB_I2C_MASTER_SLAVE;
            break;
        
        default:
            break;
    }

    /* Updating common configurations */
    scb_i2c_cfg.useRxFifo = true;
    scb_i2c_cfg.useTxFifo = true;
    scb_i2c_cfg.acceptAddrInFifo = false;
    scb_i2c_cfg.ackGeneralAddr = false;
    scb_i2c_cfg.enableWakeFromSleep = false;

    /*De-Initialize the I2C Instance*/
    Cy_SCB_I2C_DeInit(scb);

    /*SCB Initilization for I2C communication*/
    cyt_i2c_status = Cy_SCB_I2C_Init(scb, &scb_i2c_cfg, &g_stc_i2c_context[i2c_config_data->i2c_no]);
    if (CY_SCB_I2C_SUCCESS == cyt_i2c_status)
    {
        i2c_config_data->i2c_actual_frequency_configured = Cy_SCB_I2C_SetDataRate(scb, i2c_config_data->transfer_rate, i2c_clock.target_frequency);
        Cy_SCB_I2C_Enable(scb);
        ret_val = ASDK_I2C_STATUS_SUCCESS;
    }
    else
    {
        /*Return Error I2C Init failed*/
        ret_val = ASDK_I2C_ERROR_INIT_FAIL;
    }

    return ret_val;
}

asdk_errorcode_t asdk_i2c_deinit(asdk_i2c_num_t i2c_no)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;
    uint8_t scb_index;
    volatile stc_SCB_t *scb = NULL;

    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    /*Cyt specific :Mapping I2C no to the associated SCB Module*/
    scb_index = scb_i2c[i2c_no];
    scb = scb_base_ptrs[scb_index];

    /* Check already de-initialized */
    if (0 == scb->unCTRL.stcField.u1ENABLED)
    {
        return ASDK_I2C_ERROR_NOT_INITIALIZED;
    }

    /*De-Initialize the I2C Instance*/
    Cy_SCB_I2C_Disable(scb, &g_stc_i2c_context[i2c_no]);
    Cy_SCB_I2C_DeInit(scb);

    return ret_val;
}

asdk_errorcode_t asdk_i2c_slave_config_read_buf(asdk_i2c_num_t i2c_no, uint8_t *rd_buff, uint8_t rd_buff_size)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;

    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    if (NULL == rd_buff)
    {
        return ASDK_I2C_ERROR_NULL_PTR;
    }

    if (!rd_buff_size)
    {
        return ASDK_I2C_ERROR_INVALID_SIZE;
    }

    Cy_SCB_I2C_SlaveConfigReadBuf(scb_base_ptrs[scb_i2c[i2c_no]], rd_buff, rd_buff_size, &g_stc_i2c_context[i2c_no]);
    
    return ret_val;
}

asdk_errorcode_t asdk_i2c_slave_config_write_buf(asdk_i2c_num_t i2c_no, uint8_t *wr_buff, uint8_t wr_buff_size)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;

    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    if (NULL == wr_buff)
    {
        return ASDK_I2C_ERROR_NULL_PTR;
    }

    if (!wr_buff_size)
    {
        return ASDK_I2C_ERROR_INVALID_SIZE;
    }

    Cy_SCB_I2C_SlaveConfigWriteBuf(scb_base_ptrs[scb_i2c[i2c_no]], wr_buff, wr_buff_size, &g_stc_i2c_context[i2c_no]);

    return ret_val;
}

asdk_errorcode_t asdk_i2c_install_callback(asdk_i2c_num_t i2c_no, asdk_i2c_callback_fun_t callback_fun)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

    /*Check for Max I2C module*/
    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_i2c[i2c_no];
    scb = scb_base_ptrs[scb_index];

    /* store callback function */
    user_i2c_callback_fun_list[i2c_no] = callback_fun;

    /* assign hw callback function name */
    switch (i2c_no)
    {
    case 0:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c0_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 1:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c1_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 2:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c2_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 3:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c3_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 4:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c4_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 5:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c5_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 6:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c6_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    case 7:
        Cy_SCB_I2C_RegisterEventCallback(scb, (scb_i2c_handle_events_t)scb_i2c7_callback_handler, &g_stc_i2c_context[i2c_no]);
        break;
    default:
        break;
    }

    return ret_val;
}

asdk_errorcode_t asdk_i2c_master_write_non_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *send_buf, uint16_t length)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;
    cy_en_scb_i2c_status_t cy_i2c_status = CY_SCB_I2C_SUCCESS;
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

    /* check for max i2c module */
    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_i2c[i2c_no];
    scb = scb_base_ptrs[scb_index];

    if ( (scb->unI2C_CTRL.stcField.u1MASTER_MODE) && (!scb->unI2C_CTRL.stcField.u1SLAVE_MODE) )
    {
        g_stc_i2c_master_tfr_cfg[i2c_no].slaveAddress = slave_addr;
        g_stc_i2c_master_tfr_cfg[i2c_no].buffer = send_buf;
        g_stc_i2c_master_tfr_cfg[i2c_no].bufferSize = length;

        /* I2C Master Write */
        cy_i2c_status = Cy_SCB_I2C_MasterWrite(scb, &g_stc_i2c_master_tfr_cfg[i2c_no], &g_stc_i2c_context[i2c_no]);
        if (CY_SCB_I2C_SUCCESS != cy_i2c_status)
        {
            ret_val = ASDK_I2C_ERROR_WRITE_FAIL;
        }
    }
    else if ( (scb->unI2C_CTRL.stcField.u1SLAVE_MODE) && (scb->unI2C_CTRL.stcField.u1MASTER_MODE) )
    {
        /* todo: implement master-slave mode */
    }

    return ret_val;
}

asdk_errorcode_t asdk_i2c_master_write_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *send_buf, uint16_t length, uint16_t time_out)
{
    return ASDK_I2C_ERROR_FEATURE_NOT_IMPLEMENTED;
}

asdk_errorcode_t asdk_i2c_master_read_non_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *recv_buf, uint16_t length)
{
    asdk_errorcode_t ret_val = ASDK_I2C_STATUS_SUCCESS;
    cy_en_scb_i2c_status_t cy_i2c_status = CY_SCB_I2C_SUCCESS;
    volatile stc_SCB_t *scb = NULL;
    uint8_t scb_index;

     /* check for max i2c module */
    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    scb_index = scb_i2c[i2c_no];
    scb = scb_base_ptrs[scb_index];

    if ((scb->unI2C_CTRL.stcField.u1MASTER_MODE) && (!scb->unI2C_CTRL.stcField.u1SLAVE_MODE))
    {
        g_stc_i2c_master_tfr_cfg[i2c_no].slaveAddress = slave_addr;
        g_stc_i2c_master_tfr_cfg[i2c_no].buffer = recv_buf;
        g_stc_i2c_master_tfr_cfg[i2c_no].bufferSize = length;

        /* I2C Master Read */
        cy_i2c_status = Cy_SCB_I2C_MasterRead(scb, &g_stc_i2c_master_tfr_cfg[i2c_no], &g_stc_i2c_context[i2c_no]);
        if (CY_SCB_I2C_SUCCESS != cy_i2c_status)
        {
            ret_val = ASDK_I2C_ERROR_READ_FAIL;
        }
    }
    else if ( (scb->unI2C_CTRL.stcField.u1SLAVE_MODE) && (scb->unI2C_CTRL.stcField.u1MASTER_MODE) )
    {
        /* todo: implement master-slave mode */
    }

    return ret_val;
}

asdk_errorcode_t asdk_i2c_master_read_blocking(asdk_i2c_num_t i2c_no, uint8_t slave_addr, uint8_t *recv_buf, uint16_t length, uint16_t time_out)
{
    return ASDK_I2C_ERROR_FEATURE_NOT_IMPLEMENTED;
}

/**
* @todo Implement for I2C Slave
*/
asdk_errorcode_t asdk_i2c_get_transfer_status(asdk_i2c_num_t i2c_no, asdk_i2c_status_t *i2c_status)
{
    uint32_t cy_i2c_transfer_status;
    volatile stc_SCB_t *scb = NULL;
    
    if (ASDK_I2C_MAX <= i2c_no)
    {
        return ASDK_I2C_ERROR_RANGE_EXCEEDED;
    }

    scb = scb_base_ptrs[scb_i2c[i2c_no]];

    if (scb->unI2C_CTRL.stcField.u1MASTER_MODE)
    {
        /* Get and return I2C Master transfer status */
        cy_i2c_transfer_status = Cy_SCB_I2C_MasterGetStatus(scb, &g_stc_i2c_context[i2c_no]);

        switch (cy_i2c_transfer_status)
        {
            case ~CY_SCB_I2C_MASTER_BUSY:
                {
                    if (g_stc_i2c_context[i2c_no].masterRdDir)
                        *i2c_status = ASDK_I2C_STATUS_RD_COMPLETE;
                    else
                        *i2c_status = ASDK_I2C_STATUS_WR_COMPLETE;
                }
                break;
            case CY_SCB_I2C_MASTER_BUSY:
                *i2c_status = ASDK_I2C_STATUS_BUSY;
                break;
            
            default:
                {
                    if (CY_SCB_I2C_MASTER_ABORT_START & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_MASTER_ABORT_START;
                    }
                    else if (CY_SCB_I2C_MASTER_BUS_ERR & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_BUS_ERROR;
                    }
                    else if (CY_SCB_I2C_MASTER_ARB_LOST & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_ARB_LOST;
                    }
                    else if (CY_SCB_I2C_MASTER_DATA_NAK & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_MASTER_DATA_NACK;
                    }
                    else if (CY_SCB_I2C_MASTER_ADDR_NAK & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_MASTER_ADDR_NACK;
                    }
                    else if (CY_SCB_I2C_MASTER_WR_IN_FIFO & cy_i2c_transfer_status)
                    {
                        *i2c_status = ASDK_I2C_STATUS_WR_IN_FIFO;
                    }
                    else
                    {
                        *i2c_status = ASDK_I2C_STATUS_ERROR;
                    }
                }
                break;
        }

    }
    else if (scb->unI2C_CTRL.stcField.u1SLAVE_MODE)
    {
        /* Get and return I2C Slave transfer status */

    }
    else if (scb->unI2C_CTRL.stcField.u1MASTER_MODE && scb->unI2C_CTRL.stcField.u1SLAVE_MODE)
    {
        /* Get and return I2C Master-Slave transfer status */
    }
    else
    {
        /* Error */
    }

    return ASDK_I2C_STATUS_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c0_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c0_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 0;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute I2C Master mode callback */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c1_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c1_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 1;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c2_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c2_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 2;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c3_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c3_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 3;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c4_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c4_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 4;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c5_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c5_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 5;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c6_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c6_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 6;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

/*----------------------------------------------------------------------------*/
/* Function : scb_i2c7_callback_handler */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function receives data through the i2c master or slave module using non blocking method

  @param uint32_t event - SCB I2C Event type

  @return void

*/
void scb_i2c7_callback_handler(uint32_t event)
{
    uint8_t i2c_no = 7;
    asdk_i2c_status_t status = ASDK_I2C_STATUS_UNDEFINED;

    /* Execute callback_function */
    status = __asdk_get_i2c_status(i2c_no, event);
    user_i2c_callback_fun_list[i2c_no](i2c_no, g_stc_i2c_context[i2c_no].masterBuffer, g_stc_i2c_context[i2c_no].masterNumBytes, status);
}

void I2C0_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[0]], &g_stc_i2c_context[0]);
}

void I2C1_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[1]], &g_stc_i2c_context[1]);
}

void I2C2_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[2]], &g_stc_i2c_context[2]);
}

void I2C3_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[3]], &g_stc_i2c_context[3]);
}

void I2C4_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[4]], &g_stc_i2c_context[4]);
}

void I2C5_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[5]], &g_stc_i2c_context[5]);
}

void I2C6_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[6]], &g_stc_i2c_context[6]);
}

void I2C7_ISR_UserCallback(void)
{
    /* I2C interrupt handler for High-Level APIs */
    Cy_SCB_I2C_Interrupt(scb_base_ptrs[scb_i2c[7]], &g_stc_i2c_context[7]);
}

/*----------------------------------------------------------------------------*/
/* Function : __asdk_get_i2c_status */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function returns the status of the I2C module based on the event from SCB I2C module.

  @param asdk_i2c_num_t i2c_no - I2C no.
  @param uint32_t event - SCB I2C Event type

  @return asdk_i2c_status_t

*/
static asdk_i2c_status_t __asdk_get_i2c_status(asdk_i2c_num_t i2c_no, uint32_t event)
{
    if (scb_base_ptrs[scb_i2c[i2c_no]]->unI2C_CTRL.stcField.u1MASTER_MODE)
    {
        if (CY_SCB_I2C_MASTER_WR_CMPLT_EVENT == event){
            return ASDK_I2C_STATUS_WR_COMPLETE;
        }
        else if (CY_SCB_I2C_MASTER_WR_IN_FIFO_EVENT == event){
            return ASDK_I2C_STATUS_WR_IN_FIFO;
        }
        else if (CY_SCB_I2C_MASTER_RD_CMPLT_EVENT == event){
            return ASDK_I2C_STATUS_RD_COMPLETE;
        }
        else{
            if (CY_SCB_I2C_MASTER_BUSY == g_stc_i2c_context[i2c_no].masterStatus){
                return ASDK_I2C_STATUS_BUSY;
            }
            else if (CY_SCB_I2C_MASTER_ARB_LOST & g_stc_i2c_context[i2c_no].masterStatus){
                return ASDK_I2C_STATUS_ARB_LOST;
            }
            else if (CY_SCB_I2C_MASTER_BUS_ERR & g_stc_i2c_context[i2c_no].masterStatus){
                return ASDK_I2C_STATUS_BUS_ERROR;
            }
            else{
                return ASDK_I2C_STATUS_ERROR;
            }
        }
    }
    else if (scb_base_ptrs[scb_i2c[i2c_no]]->unI2C_CTRL.stcField.u1SLAVE_MODE)
    {
        if (CY_SCB_I2C_SLAVE_WR_CMPLT_EVENT == event){
            return ASDK_I2C_STATUS_WR_COMPLETE;
        }
        else if (CY_SCB_I2C_SLAVE_RD_CMPLT_EVENT == event){
            return ASDK_I2C_STATUS_RD_COMPLETE;
        }
        else{
            if ((CY_SCB_I2C_SLAVE_WR_BUSY & g_stc_i2c_context[i2c_no].slaveStatus) || 
                (CY_SCB_I2C_SLAVE_RD_BUSY & g_stc_i2c_context[i2c_no].slaveStatus)){
                return ASDK_I2C_STATUS_BUSY;
            }
            else if (CY_SCB_I2C_SLAVE_ARB_LOST & g_stc_i2c_context[i2c_no].slaveStatus){
                return ASDK_I2C_STATUS_ARB_LOST;
            }
            else if (CY_SCB_I2C_SLAVE_BUS_ERR & g_stc_i2c_context[i2c_no].slaveStatus){
                return ASDK_I2C_STATUS_BUS_ERROR;
            }
            else{
                return ASDK_I2C_STATUS_ERROR;
            }
        }
    }
    else
    {
        /* Execute I2C Master-Slave mode callback */
        //todo:
    }

    return ASDK_I2C_STATUS_UNDEFINED;
}
