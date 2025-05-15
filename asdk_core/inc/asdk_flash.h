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
    asdk_flash.h

    @path
    inc/asdk_flash.h

    @Created on
    Sep 5, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the Flash Module for Ather SDK(asdk).
*/

#ifndef ASDK_FLASH_H
#define ASDK_FLASH_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_error.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
/*!
 * @brief Flash erase type
 */
typedef enum
{
    ASDK_FLASH_ERASETYPE_ENTIRE_FLASH = 0,
    ASDK_FLASH_ERASETYPE_SECTOR_WISE,
    ASDK_FLASH_ERASETYPE_MAX,
    ASDK_FLASH_ERASETYPE_INVALID = ASDK_FLASH_ERASETYPE_MAX,

} asdk_flash_erase_type_t;

/*!
 * @brief Which bank mode the flash should be used Single/Dual bank mode
 */
typedef enum
{
    ASDK_FLASH_BANKTYPE_SINGLE_BANK = 0,
    ASDK_FLASH_BANKTYPE_DUAL_BANK,
    ASDK_FLASH_BANKTYPE_MAX,
    ASDK_FLASH_BANKTYPE_INVALID = ASDK_FLASH_BANKTYPE_MAX,

} asdk_flash_bank_type_t;

/*!
 * @brief Tells if in dual bank mode which Bank to erase.
 */
typedef enum
{
    ASDK_FLASH_BANK_A = 0,
    ASDK_FLASH_BANK_B,
    ASDK_FLASH_BANK_MAX,
    ASDK_FLASH_BANK_INVALID = ASDK_FLASH_BANK_MAX,

} asdk_flash_dual_bank_t;

/*!
 * @brief Which flash should be initialized code flash/work flash/both
 */
typedef enum
{
    ASDK_FLASH_INIT_FLASHTYPE_BOTH_CODE_FLASH_DATA_FLASH = 0,
    ASDK_FLASH_INIT_FLASHTYPE_CODE_FLASH,
    ASDK_FLASH_INIT_FLASHTYPE_DATA_FLASH,
    ASDK_FLASH_INIT_FLASHTYPE_MAX,
    ASDK_FLASH_INIT_FLASHTYPE_INVALID = ASDK_FLASH_INIT_FLASHTYPE_MAX,

} asdk_flash_type_t;

/*!
 * @brief Flash operation type
 *
 * @note:The DMA support has not been added in the current version
 */
typedef enum
{
    ASDK_FLASH_OPERATION_NON_BLOCKING_MODE = 0,
    ASDK_FLASH_OPERATION_BLOCKING_MODE,
    ASDK_FLASH_OPERATION_MAX,
    ASDK_FLASH_OPERATION_INVALID = ASDK_FLASH_OPERATION_MAX,
} asdk_flash_operation_mode_t;
/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/
/*!
 * @brief The initialization configuration structure to be passed during intialization
 *
 * @note:For cyt expects operation mode in the init otherwise it would be ignored
 */
typedef struct
{
    asdk_flash_type_t flash_type;                     /*Initialize the code flash/data flash/both*/
    asdk_flash_operation_mode_t flash_operation_mode; /*Initialize the flash in the polling mode or interrupt mode*/
} asdk_flash_init_deinit_config_t;

/*!
 * @brief This configuration structure implements the flash read/write config structure
 */
typedef struct
{
    uint32_t size_in_bytes;                           /* Size of variable to be read or write*/
    uint32_t source_addr;                             /*Source address of the varaible to be read/write*/
    uint32_t destination_addr;                        /*Destination address for the varaible to be read / write*/
    asdk_flash_operation_mode_t flash_operation_mode; /*Tells about the flash operation mode interrupt or polling*/
} asdk_flash_operation_config_t;

/*!
 * @brief This structure implements the erase configuration structure
 *
 * @note:Incase of single bank mode the asdk_flash_dual_bank_t which_bank will be ignored
 */
typedef struct
{
    asdk_flash_erase_type_t EraseType; /*!< Mass erase or sector Erase.*/
    asdk_flash_bank_type_t Banks;      /*Select flash to be erased incase of single bank or dual bank mode*/
    asdk_flash_dual_bank_t which_bank; /*!< Select banks to erase when Mass erase is enabled. */
    uint32_t Base_Sector_Address;                   /*!< Initial Address of  FLASH sector to erase when Entire Flash Erase is disabled*/
    uint32_t NbSectors;                /*!< Number of sectors to be erased.This parameter must be a value between 1 and (max number of sectors - value of Initial sector)*/
    asdk_flash_operation_mode_t flash_operation_mode;

} asdk_flash_erase_config_t;

/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/
typedef void (*asdk_flash_callback_fun_t)(void);

/* ************************************************************************** *
 *                          Configuration APIs                                *
 * ************************************************************************** */
/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the Flash Module based on the config structure passed as parameter.

  @param [in] flash_config ASDK Flash configuration parameter.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_STATUS_ERROR
    - @ref ASDK_FLASH_STATUS_BUSY
    - @ref ASDK_FLASH_STATUS_TIMEOUT
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INIT_FAIL
    - @ref ASDK_FLASH_ERROR_DEINIT_FAIL
    - @ref ASDK_FLASH_ERROR_NOT_IMPLEMENTED
    - @ref ASDK_FLASH_ERROR_NOT_INITIALIZED
    - @ref ASDK_FLASH_ERROR_INVALID_HANDLER
    - @ref ASDK_FLASH_ERROR_INVALID_DATA_SIZE
    - @ref ASDK_FLASH_ERROR_INVALID_SECTOR_SIZE
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
    - @ref ASDK_FLASH_ERROR_INVALID_DATA_ALIGNMENT
*/
asdk_errorcode_t asdk_flash_init(asdk_flash_init_deinit_config_t *flash_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the Flash Module based on the config structure passed as parameter.

  @param [in] flash_config ASDK Flash configuration structure

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_TYPE
*/
asdk_errorcode_t asdk_flash_deinit(asdk_flash_init_deinit_config_t *flash_config);


/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_install_callback */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function installs/registers the callback function for the user callback.

  @param [in] callback_fun Callback function to the user.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
*/
asdk_errorcode_t asdk_flash_install_callback(asdk_flash_callback_fun_t callback_fun);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief
This function enables the user to read the flash memory based on the address passed in the config structure

  @param [in] flash_read_config Flash input config structure for read operation.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_read(asdk_flash_operation_config_t *flash_read_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief
   This function enables the user to write to the flash memory based on the address passed in the config structure

  @param [in] flash_write_config- Flash input config structure for write operation.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_write(asdk_flash_operation_config_t *flash_write_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_all_block */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the whole flash memory except the protected blocks.

  @param [in] flash_erase_config Flash erase config structure

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_all_block(asdk_flash_erase_config_t *flash_erase_config);

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_erase_sector */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the flash memory sectorwise based on the config structure passed.

  @param [in] flash_erase_config Flash erase config structure

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE
    - @ref ASDK_FLASH_ERROR_NULL_PTR
    - @ref ASDK_FLASH_STATUS_ERROR
*/
asdk_errorcode_t asdk_flash_erase_sector(asdk_flash_erase_config_t *flash_erase_config);

/*******************************************************************************
 * Function Cy_Is_SROM_API_Completed
 ****************************************************************************/
/**
 
  @brief A Function for user to know whether CM0+ completed erase/program Flash or not.
        In non-blocking mode, it return "true" always.
 
  @retval True, CM0+ has completed requested SROM API.
  @return False, CM0+ has not completed requested SROM API yet.
 
 *******************************************************************************/
bool asdk_Is_SROM_API_Completed();

/*----------------------------------------------------------------------------*/
/* Function : asdk_flash_get_sector_size */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function erases the flash memory sectorwise based on the config structure passed.

  @param [in] address Address of which the user wants to know the sector size
  @param [out] flash_sector_size Variable in which the sector size would be given to the user.

  @return
    - @ref ASDK_FLASH_STATUS_SUCCESS
    - @ref ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS
*/
asdk_errorcode_t asdk_flash_get_sector_size(uint32_t address, uint32_t *flash_sector_size);
#endif /* ASDK_FLASH_H */
