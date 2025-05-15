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
    asdk_emulated_eeprom.h

    @path
    inc/asdk_emulated_eeprom.h

    @Created on
    Sep 24, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file prototypes the Emulated EEPROM Module in Flash for Ather SDK(asdk).

    @note
    This eeprom module is for updating a global structure and its members as the
    virtual address are mapped to each variable of the structure and is being taken
    care inside the DAL of emulated eeprom module.
    This eeprom module developed is for the emulation of upto 4KB (4*1024 Bytes).
*/

#ifndef ASDK_EMULATED_EEPROM_H
#define ASDK_EMULATED_EEPROM_H

/*==============================================================================

                               INCLUDE FILES

==============================================================================*/

#include <stdint.h>
#include <stdbool.h>
#include "asdk_error.h"

/*==============================================================================

                      DEFINITIONS AND TYPES : MACROS

==============================================================================*/
/* Define the size of the sectors to be used */
#define EMULATED_EEPROM_PAGE_SIZE (uint32_t)0x4000 /* Page size = 16KByte */

/* EEPROM start address in Flash */
#define EEPROM_START_ADDRESS ((uint32_t)0x14000000) /* EEPROM emulation start address:Work Flash Sector0 memory */

/* Pages 0 and 1 base and end addresses */
#define PAGE0_BASE_ADDRESS ((uint32_t)(EEPROM_START_ADDRESS + 0x0000))
#define PAGE0_END_ADDRESS ((uint32_t)(EEPROM_START_ADDRESS + (EMULATED_EEPROM_PAGE_SIZE - 1)))
#define PAGE0_ID PAGE0_BASE_ADDRESS

#define PAGE1_BASE_ADDRESS ((uint32_t)(EEPROM_START_ADDRESS + 0x4000))
#define PAGE1_END_ADDRESS ((uint32_t)(EEPROM_START_ADDRESS + (2 * EMULATED_EEPROM_PAGE_SIZE - 1)))
#define PAGE1_ID PAGE1_BASE_ADDRESS

/* Used Flash pages for EEPROM emulation */
#define PAGE0 ((uint16_t)0x0000)
#define PAGE1 ((uint16_t)0x0001) /* Page nb between PAGE0_BASE_ADDRESS & PAGE1_BASE_ADDRESS*/

/* No valid page define */
#define NO_VALID_PAGE ((uint16_t)0x00AB)

/* Page status definitions */
#define ERASED ((uint32_t)0xFFFFFFFF)       /* Page is empty */
#define RECEIVE_DATA ((uint32_t)0xEEEEEEEE) /* Page is marked to receive data */
#define VALID_PAGE ((uint32_t)0x00000000)   /* Page containing valid data */
#define INVALID_PAGE ((uint32_t)0x66666666) /*Page containing invalid data*/

/* Valid pages in read and write defines */
#define READ_FROM_VALID_PAGE ((uint8_t)0x00)
#define WRITE_IN_VALID_PAGE ((uint8_t)0x01)

/* Page full define */
#define PAGE_FULL ((uint8_t)0x80)

#define INITIAL_VALUE ((uint8_t)0x01)
#define INITIAL_VALUE_ZERO ((uint8_t)0x00)
#define INITIAL_PAGE_STATUS   ((uint32_t)0x66666666)
/*==============================================================================

                      DEFINITIONS AND TYPES : ENUMS

==============================================================================*/
/*!
 * @brief Which flash should be initialized code flash/work flash/both for the eeprom emulation
 * @brief Basically which flash will be used for the eeprom emulation
 * Implements : asdk_emulated_eeprom_type_t
 *
 * @note:
 */
typedef enum
{
    ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_BOTH_CODE_FLASH_DATA_FLASH = 0,
    ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_CODE_FLASH,
    ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_DATA_FLASH,
    ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_MAX,
    ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_INVALID = ASDK_EMULATED_EEPROM_INIT_FLASHTYPE_MAX,

} asdk_emulated_eeprom_type_t;

/*!
 * @brief Flash operation type
 *
 * Implements : asdk_flash_operation_mode_t
 *
 * @note:The DMA support has not been added in the current version
 */
typedef enum
{
    ASDK_EMULATED_EEPROM_OPERATION_NON_BLOCKING_MODE = 0,
    ASDK_EMULATED_EEPROM_OPERATION_BLOCKING_MODE,
    ASDK_EMULATED_EEPROM_OPERATION_MAX,
    ASDK_EMULATED_EEPROM_OPERATION_INVALID = ASDK_EMULATED_EEPROM_OPERATION_MAX,
} asdk_emulated_eeprom_operation_mode_t;

/*!
 * @brief Flash operation type
 *
 * Implements : asdk_flash_operation_mode_t
 *
 * @note:The DMA support has not been added in the current version
 */
typedef enum
{
    ASDK_EMULATED_EEPROM_SIZE_2KB = 0,
    ASDK_EMULATED_EEPROM_SIZE_4KB,
    ASDK_EMULATED_EEPROM_SIZE_8KB,
    ASDK_EMULATED_EEPROM_SIZE_MAX,
    ASDK_EMULATED_EEPROM_SIZE_INVALID = ASDK_EMULATED_EEPROM_SIZE_MAX,
} asdk_emulated_eeprom_size_t;
/*==============================================================================

                    LOCAL DEFINITIONS AND TYPES : STRUCTURES

==============================================================================*/
/*!
 * @brief The initialization configuration structure to be passed during intialization
 *
 * Implements : asdk_emulated_eeprom_init_deinit_config_t
 *
 * @note:
 */
typedef struct
{
    asdk_emulated_eeprom_type_t  flash_type_used;                              /*Initialize the code flash/data flash/both*///name to be chnaged
    asdk_emulated_eeprom_operation_mode_t emulated_eeprom_operation_mode; /*Initialize the flash in the polling mode or interrupt mode*/
    asdk_emulated_eeprom_size_t emulated_eeprom_size;
} asdk_emulated_eeprom_init_deinit_config_t;


/*!
 * @brief The global structure to keep track of the last valid address updated
 *
 * Implements : eeprom_global_variables_t
 *
 * @note:This structure will be used for updating the latest address, size of global structure 
 */
typedef struct
{
    uint32_t Destination_Address;
    uint32_t Var_Size_Backup;
    uint32_t Page_Status;
    uint32_t *structure_base_address_ptr;
} eeprom_global_variables_t;
/*==============================================================================

                           EXTERNAL DECLARATIONS

==============================================================================*/

/*==============================================================================

                           FUNCTION PROTOTYPES

==============================================================================*/
typedef void (*asdk_emulated_eeprom_callback_fun_t)(void);

/* ************************************************************************** *
 *                          Configuration APIs                                *
 * ************************************************************************** */
/*----------------------------------------------------------------------------*/
/* Function : asdk_emulated_eeprom_init */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function initializes the Flash Module based on the config structure passed as parameter.

  @param asdk_emulated_eeprom_init_deinit_config_t flash_config ( input ) - ASDK Flash configuration parameter.

  @retval ASDK_EMULATED_EEPROM_STATUS_SUCCESS
  @retval ASDK_EMULATED_EEPROM_STATUS_ERROR
  @retval ASDK_EMULATED_EEPROM_STATUS_BUSY
  @retval ASDK_EMULATED_EEPROM_STATUS_TIMEOUT
  @retval ASDK_EMULATED_EEPROM_ERROR_NULL_PTR
  @retval ASDK_EMULATED_EEPROM_ERROR_INIT_FAIL
  @retval ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL
  @retval ASDK_EMULATED_EEPROM_ERROR_NOT_IMPLEMENTED
  @retval ASDK_EMULATED_EEPROM_ERROR_NOT_INITIALIZED
  @retval ASDK_EMULATED_EEPROM_ERROR_INVALID_HANDLER
  @retval ASDK_EMULATED_EEPROM_ERROR_INVALID_DATA_SIZE
  @retval ASDK_EMULATED_EEPROM_ERROR_INVALID_SECTOR_SIZE
  @retval ASDK_EMULATED_EEPROM_ERROR_INVALID_FLASH_ADDRESS
  @retval ASDK_EMULATED_EEPROM_ERROR_INVALID_DATA_ALIGNMENT

  @note

*/
asdk_errorcode_t asdk_emulated_eeprom_init(asdk_emulated_eeprom_init_deinit_config_t *eeeprom_init_config, uint8_t *global_structure_address, uint32_t global_structure_size);

/*----------------------------------------------------------------------------*/
/* Function : asdk_emulated_eeprom_deinit */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function de-initializes the Flash Module based on the config structure passed as parameter.

  @param asdk_emulated_eeprom_init_deinit_config_t flash_config ( input ) - ASDK Flash configuration parameter.

  @retval ASDK_EMULATED_EEPROM_STATUS_SUCCESS
  @retval ASDK_EMULATED_EEPROM_STATUS_ERROR
  @retval ASDK_EMULATED_EEPROM_STATUS_BUSY
  @retval ASDK_EMULATED_EEPROM_STATUS_TIMEOUT
  @retval ASDK_EMULATED_EEPROM_ERROR_NULL_PTR
  @retval ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL

  @note

*/
asdk_errorcode_t asdk_emulated_eeprom_deinit(asdk_emulated_eeprom_init_deinit_config_t *eeeprom_deinit_config);


/*----------------------------------------------------------------------------*/
/* Function : asdk_emulated_eeprom_read */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function reads the Emulated EEPROM variable (of which the address is passed as parameter)

  @param uint8_t* read_back_var(input)           - base address of the varaible of which the value needs to be read
  @param uint8_t* structure_member_addr( input ) - base address of the variable to be updated.
  @param uint8_t* global_structure_addr(input)   - base address of the global structure for address reference
  @param uint32_t structure_member_size(input)   - size of the structure member to be updated


  @retval ASDK_EMULATED_EEPROM_STATUS_SUCCESS
  @retval ASDK_EMULATED_EEPROM_STATUS_ERROR
  @retval ASDK_EMULATED_EEPROM_STATUS_BUSY
  @retval ASDK_EMULATED_EEPROM_STATUS_TIMEOUT
  @retval ASDK_EMULATED_EEPROM_ERROR_NULL_PTR
  @retval ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL

  @note

*/
asdk_errorcode_t asdk_emulated_eeprom_read(uint8_t *read_back_var, uint8_t *structure_member_addr, uint8_t *global_structure_addr, uint32_t structure_member_size);


/*----------------------------------------------------------------------------*/
/* Function : asdk_emulated_eeprom_write */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function writes to the Emulated EEPROM whenever a member of the structure needs to be updated
  or the entire structure needs to be updated.
  Can be used as alternate to the EEPROM Upadate function.

  @param uint8_t* structure_member_addr( input ) - base address of the variable to be updated.
  @param uint8_t* global_structure_addr(input)   - base address of the global structure for address reference
  @param uint32_t structure_member_size(input)   - size of the structure member to be updated


  @retval ASDK_EMULATED_EEPROM_STATUS_SUCCESS
  @retval ASDK_EMULATED_EEPROM_STATUS_ERROR
  @retval ASDK_EMULATED_EEPROM_STATUS_BUSY
  @retval ASDK_EMULATED_EEPROM_STATUS_TIMEOUT
  @retval ASDK_EMULATED_EEPROM_ERROR_NULL_PTR
  @retval ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL

  @note

*/
asdk_errorcode_t asdk_emulated_eeprom_write(uint8_t *structure_member_addr, uint8_t *global_structure_addr, uint32_t structure_member_size, uint32_t global_structure_size);


/*----------------------------------------------------------------------------*/
/* Function : asdk_emulated_eeprom_update */
/*----------------------------------------------------------------------------*/
/*!
  @brief
  This function updates the Emulated EEPROM whenever a member of the structure needs to be updated
  or the entire structure needs to be updated.

  @param uint8_t* structure_member_addr( input ) - base address of the variable to be updated.
  @param uint8_t* global_structure_addr(input)   - base address of the global structure for address reference
  @param uint32_t structure_member_size(input)   - size of the structure member to be updated


  @retval ASDK_EMULATED_EEPROM_STATUS_SUCCESS
  @retval ASDK_EMULATED_EEPROM_STATUS_ERROR
  @retval ASDK_EMULATED_EEPROM_STATUS_BUSY
  @retval ASDK_EMULATED_EEPROM_STATUS_TIMEOUT
  @retval ASDK_EMULATED_EEPROM_ERROR_NULL_PTR
  @retval ASDK_EMULATED_EEPROM_ERROR_DEINIT_FAIL

  @note

*/
asdk_errorcode_t asdk_emulated_eeprom_update(uint8_t *structure_member_addr, uint8_t *global_structure_addr, uint32_t structure_member_size, uint32_t global_structure_size);

#endif /* ASDK_EMULATED_EEPROM_H */
