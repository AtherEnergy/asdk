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
    asdk_emulated_eeprom.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_emulated_eeprom.c

    @Created on
    Sep 25, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file contains the implementation of the emulated EEPROM module for the CYT2B75 microcontroller.

    @note
    This eeprom module is for updating a global structure and its members as the
    virtual address are mapped to each variable of the structure and is being taken
    care inside the DAL of emulated eeprom module.
    This eeprom module developed is for the emulation of upto 4KB (4*1024 Bytes).

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_mcu_pins.h" // Defines MCU pins as is
#include "asdk_emulated_eeprom.h"
#include "asdk_flash.h"

// sdk includes
#include "cy_device_headers.h" // Defines reg. and variant of CYT2B7 series
#include "scb/cy_scb_spi.h"    // CYT2B75 GPIO driver APIs
#include "flash/cy_flash.h"
#include "sysclk/cy_sysclk.h"
#include "sysint/cy_sysint.h" // CYT2B75 system Interrupt APIs

/*==============================================================================

                        LOCAL AND EXTERNAL DEFINITIONS

==============================================================================*/
eeprom_global_variables_t g_eeprom_variables;

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
/*Internal to EEPROM DAL functions*/
static inline bool __asdk_emulated_eeprom_format();
static inline uint16_t __asdk_emulated_eeprom_findvalidpage(uint16_t Operation);
static inline uint16_t __asdk_emulated_eeprom_verifypagefullwritevariable(uint16_t VirtualAddress, uint16_t Data);
static inline bool __asdk_emulated_eeprom_pagetransfer(uint8_t *struct_member_add, uint8_t *structure_address, uint32_t struct_member_address, uint32_t g_structure_size);
static inline bool __asdk_emulated_eeprom_verifypagefullyerased(uint32_t address);
static inline cy_en_flashdrv_status_t __asdk_flash_blank_check(uint32_t address, uint32_t no_of_var);

/*******************************************************************************
 * Function __asdk_workflash_blank_check
 ****************************************************************************/
/**
 *
 * This function is used to check if the work flash address is in erased state or not
 * This function will be internal to DAL of flash and is very much cyt specific
 *
 *******************************************************************************/
/**
 * @brief  Verifies if the Address passed as argument is Erased or not.
 * @param  address: 32 bit address of the page
 * @param  size: Number of words to be checked
 * @retval Blank_Check_Status
 */

static inline cy_en_flashdrv_status_t __asdk_flash_blank_check(uint32_t address, uint32_t size)
{

    cy_en_flashdrv_status_t blank_status = CY_FLASH_DRV_SUCCESS;
    cy_stc_flash_blankcheck_config_t blankCheckConfig;
    blankCheckConfig.addrToBeChecked = (uint32_t *)address;
    blankCheckConfig.numOfWordsToBeChecked = size;
    blank_status = Cy_Flash_BlankCheck(NULL, &blankCheckConfig, CY_FLASH_ERASESECTOR_BLOCKING);
    return blank_status;
}

/*******************************************************************************
 * Function __asdk_workflash_blank_check
 ******************************************************************************/
/********************************************************************************
 * This function is used to check if the work flash address is in erased state or not
 * This function will be internal to DAL of flash and is very much cyt specific
 *******************************************************************************/
/*******************************************************************************
 * @brief  Verifies if the Address passed as argument is Erased or not.
 * @param  address: 32 bit address of the page
 * @param  size: Number of words to be checked
 * @retval Blank_Check_Status
 *********************************************************************************/
static inline bool __asdk_emulated_eeprom_format()
{

    // bool EraseStatus = true;
    asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

    asdk_flash_erase_config_t flash_erase_config_t = {
        .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
        .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
        .Base_Sector_Address = PAGE0_BASE_ADDRESS,
        .NbSectors = 8,
    };
    /* Erase Page0 */
    if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE0_BASE_ADDRESS))
    {
        flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
    }

    /* Set Page0 as valid page: Write VALID_PAGE at Page0 base address */
    /* Mark Page0 as valid */
    g_eeprom_variables.Page_Status = VALID_PAGE;
    g_eeprom_variables.Destination_Address = (PAGE0_BASE_ADDRESS + 4);

    asdk_flash_operation_config_t flash_write_config_t = {
        .size_in_bytes = 4,
        .source_addr = (uint32_t)&g_eeprom_variables.Page_Status,
        .destination_addr = PAGE0_BASE_ADDRESS,
        .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
    };

    flash_error_t = asdk_flash_write(&flash_write_config_t);

    flash_erase_config_t.Base_Sector_Address = PAGE1_BASE_ADDRESS;
    /* Erase Page1 */
    if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE1_BASE_ADDRESS))
    {

        flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
    }

    return flash_error_t;
}

/***************************************************************************************
 * @brief  Verify if specified page is fully erased.
 * @param  Address: page address
 *   This parameter can be one of the following values:
 *     @arg PAGE0_BASE_ADDRESS: Page0 base address
 *     @arg PAGE1_BASE_ADDRESS: Page1 base address
 * @retval page fully erased status:
 *           - 0: if Page not erased
 *           - 1: if Page erased
 ****************************************************************************************/
static inline bool __asdk_emulated_eeprom_verifypagefullyerased(uint32_t Address)
{
    bool ReadStatus = (bool)INITIAL_VALUE;
    uint32_t AddressValue = PAGE0_BASE_ADDRESS;
    cy_en_flashdrv_status_t Blank_Check_Status = CY_FLASH_DRV_SUCCESS;

    /* Check each active page address starting from end */
    if ((PAGE0_BASE_ADDRESS <= Address) && (PAGE0_END_ADDRESS >= Address))
    {
        while (Address <= PAGE0_END_ADDRESS)
        {

            /* Get the current location content to be compared with virtual address */
            Blank_Check_Status = __asdk_flash_blank_check(Address, 1);
            if (CY_FLASH_DRV_SUCCESS == Blank_Check_Status)
            {
                AddressValue = ERASED;
            }

            /* Compare the read address with the virtual address */
            if (AddressValue != ERASED)
            {

                /* In case variable value is read, reset ReadStatus flag */
                ReadStatus = 0;

                break;
            }
            /* Next address location */
            Address = Address + 4;
        }
    }
    else if ((PAGE1_BASE_ADDRESS <= Address) && (PAGE1_END_ADDRESS >= Address))
    {
        while (Address <= PAGE1_END_ADDRESS)
        {
            Blank_Check_Status = __asdk_flash_blank_check(Address, 1);
            if (CY_FLASH_DRV_SUCCESS == Blank_Check_Status)
            {
                AddressValue = ERASED;
            }

            /* Compare the read address with the virtual address */
            if (AddressValue != ERASED)
            {

                /* In case variable value is read, reset ReadStatus flag */
                ReadStatus = 0;

                break;
            }
            /* Next address location */
            Address = Address + 4;
        }
    }
    /* Return ReadStatus value: (0: Page not erased, 1: Sector erased) */
    return ReadStatus;
}
/**
 * @brief  Find valid Page for write or read operation
 * @param  Operation: operation to achieve on the valid page.
 *   This parameter can be one of the following values:
 *     @arg READ_FROM_VALID_PAGE: read operation from valid page
 *     @arg WRITE_IN_VALID_PAGE: write operation from valid page
 * @retval Valid page number (PAGE or PAGE1) or NO_VALID_PAGE in case
 *   of no valid page was found
 */
static inline uint16_t __asdk_emulated_eeprom_findvalidpage(uint16_t Operation)
{
    cy_en_flashdrv_status_t Blank_Check_Status = (bool)INITIAL_VALUE;
    uint32_t Valid_PageStatus0 = INITIAL_PAGE_STATUS;
    uint32_t Valid_PageStatus1 = INITIAL_PAGE_STATUS;

    Blank_Check_Status = __asdk_flash_blank_check(PAGE0_BASE_ADDRESS, 1);
    /* Get Page0 actual status */
    if (CY_FLASH_DRV_SUCCESS != Blank_Check_Status)
    {
        Valid_PageStatus0 = ((uint32_t *)PAGE0_BASE_ADDRESS)[0];
    }
    else
    {
        Valid_PageStatus0 = ERASED;
    }

    Blank_Check_Status = __asdk_flash_blank_check(PAGE1_BASE_ADDRESS, 1);
    /* Get Page1 actual status */
    if (CY_FLASH_DRV_SUCCESS != Blank_Check_Status)
    {
        Valid_PageStatus1 = ((uint32_t *)PAGE1_BASE_ADDRESS)[0];
    }
    else
    {
        Valid_PageStatus1 = ERASED;
    }

    /* Write or read operation */
    switch (Operation)
    {
    case READ_FROM_VALID_PAGE:
        if (Valid_PageStatus0 == VALID_PAGE)
        {
            return PAGE0; /* Page0 valid */
        }
        else if (Valid_PageStatus1 == VALID_PAGE)
        {
            return PAGE1; /* Page1 valid */
        }
        else
        {
            return NO_VALID_PAGE; /* No valid Page */
        }
        break;
    case WRITE_IN_VALID_PAGE:
        if (VALID_PAGE == Valid_PageStatus1)
        {
            /* Page0 receiving data */
            if (ERASED == Valid_PageStatus0)
            {
                return PAGE1; /* Page0 valid */
            }
        }
        else if (VALID_PAGE == Valid_PageStatus0)
        {
            /* Page1 receiving data */
            if (ERASED == Valid_PageStatus1)
            {

                return PAGE0; /* Page1 valid */
            }
        }
        else
        {
            return NO_VALID_PAGE;
        }
        break;

    default:
        return PAGE0;
        break;
    }

    return PAGE0;
}

/**
 * @brief  Verify if active page is full and Writes variable in EEPROM.
 * @param  VirtAddress: 16 bit virtual address of the variable
 * @param  Data: 16 bit data to be written as variable value
 * @retval Success or error status:
 *           - FLASH_COMPLETE: on success
 *           - PAGE_FULL: if valid page is full
 *           - NO_VALID_PAGE: if no valid page was found
 *           - Flash error code: on write Flash error
 */

static inline uint16_t __asdk_emulated_eeprom_verifypagefullwritevariable(uint16_t VirtAddress, uint16_t Data)
{

    uint16_t FlashStatus = INITIAL_VALUE;
    asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

    uint16_t ValidPage = PAGE0;
    // uint32_t address;
    uint32_t Word_To_Write;

    uint32_t pageendaddress = PAGE0_BASE_ADDRESS + EMULATED_EEPROM_PAGE_SIZE;

    if ((PAGE0_BASE_ADDRESS <= g_eeprom_variables.Destination_Address) && (PAGE0_END_ADDRESS >= g_eeprom_variables.Destination_Address))
    {
        ValidPage = PAGE0;
    }
    else if ((PAGE1_BASE_ADDRESS <= g_eeprom_variables.Destination_Address) && (PAGE1_END_ADDRESS >= g_eeprom_variables.Destination_Address))
    {
        ValidPage = PAGE1;
    }
    else
    {
        ValidPage = NO_VALID_PAGE;
    }

    /* Get the valid Page start address */
    /*Below address is defined only for vlaidation purpose*/
    // address = PAGE0_BASE_ADDRESS, 
    // address = (uint32_t)(PAGE0_BASE_ADDRESS + (uint32_t)(ValidPage * EMULATED_EEPROM_PAGE_SIZE));
    // pageendaddress = address + 80;
    pageendaddress = (uint32_t)((PAGE0_BASE_ADDRESS - 1) + (uint32_t)((ValidPage + 1) * EMULATED_EEPROM_PAGE_SIZE));

    while (g_eeprom_variables.Destination_Address < pageendaddress)
    {
        {

            Word_To_Write = VirtAddress | ((Data) << 16);
            asdk_flash_operation_config_t flash_write_config_t = {
                .size_in_bytes = 4,
                .source_addr = (uint32_t)(&Word_To_Write),
                .destination_addr = g_eeprom_variables.Destination_Address,
                .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
            };

            // Handling the last 4 byte write in the end address of the page
            if (pageendaddress <= (g_eeprom_variables.Destination_Address + 4))
            {
                break;
            }

            flash_error_t = asdk_flash_write(&flash_write_config_t);
            if(ASDK_FLASH_STATUS_SUCCESS == flash_error_t)
            {
                FlashStatus = INITIAL_VALUE_ZERO;
            }
            else
            {
                FlashStatus = INITIAL_VALUE;

            }
            g_eeprom_variables.Destination_Address = g_eeprom_variables.Destination_Address + 4;

            /* Return program operation status */
            return FlashStatus;
        }
    }

    /* Return PAGE_FULL in case the valid page is full */
    return PAGE_FULL;
}

/**
 * @brief  Transfers last updated variables data from the full Page to
 *   an empty one.
 * @param  VirtAddress: 16 bit virtual address of the variable
 * @param  Data: 16 bit data to be written as variable value
 * @retval Success or error status:
 *           - FLASH_COMPLETE: on success
 *           - PAGE_FULL: if valid page is full
 *           - NO_VALID_PAGE: if no valid page was found
 *           - Flash error code: on write Flash error
 */

static inline bool __asdk_emulated_eeprom_pagetransfer(uint8_t *var_source, uint8_t *struct_source, uint32_t var_size, uint32_t struct_size)
{
    uint16_t FlashStatus = INITIAL_VALUE_ZERO;
    uint32_t NewPageAddress = PAGE0_BASE_ADDRESS;
    uint32_t OldPageID = INITIAL_VALUE_ZERO;
    uint32_t Iteration_Loop = INITIAL_VALUE;
    uint32_t Oldpage = INITIAL_VALUE_ZERO;
    uint32_t Data_To_Write = INITIAL_VALUE_ZERO;
    uint16_t ValidPage = PAGE0;
    uint32_t addr_offset = INITIAL_VALUE_ZERO;

    cy_en_flashdrv_status_t Blank_Check_Status = CY_FLASH_DRV_SUCCESS;
    uint8_t arr_for_page_transfer[g_eeprom_variables.Var_Size_Backup];
    uint32_t size_of_var_to_transfer = g_eeprom_variables.Var_Size_Backup;
    asdk_errorcode_t flash_error_status = ASDK_FLASH_STATUS_SUCCESS;

    /*Variables for the addr offset  and var size to be 2bytes offset*/
    addr_offset = struct_source - var_source;
    var_size = var_size + (var_size % 2);
    addr_offset = addr_offset - (addr_offset % 2);

    /*Get the Valid Page from which data will be transferred to the new page*/
    ValidPage = __asdk_emulated_eeprom_findvalidpage(READ_FROM_VALID_PAGE);
    if (PAGE1 == ValidPage) /* Page1 valid */
    {
        /* New page address where variable will be moved to */
        NewPageAddress = PAGE0_BASE_ADDRESS;

        /* Old page ID where variable will be taken from */
        OldPageID = PAGE1_ID;
        OldPageID = OldPageID + 4;
    }
    else if (PAGE0 == ValidPage) /* Page0 valid */
    {
        /* New page address  where variable will be moved to */
        NewPageAddress = PAGE1_BASE_ADDRESS;

        /* Old page ID where variable will be taken from */
        OldPageID = PAGE0_ID;
        OldPageID = OldPageID + 4;
    }
    else
    {
        return NO_VALID_PAGE; /* No valid Page */
    }

    /* Mark Page0 as valid */
    asdk_flash_operation_config_t flash_write_config_t = {0};
    flash_write_config_t.size_in_bytes = 4;
    flash_write_config_t.source_addr = (uint32_t)(&g_eeprom_variables.Page_Status);
    flash_write_config_t.destination_addr = PAGE0_BASE_ADDRESS;
    flash_write_config_t.flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE;

    /*EEPROM Page Transfer from OldPage to NewPage*/
    // /********************************************************************/
    // /*Updating the Variable Size and Address Offset if not 2Bytes aligned*/
    // addr_offset = addr_offset - (addr_offset % 2);
    // /*******************************************************************/
    uint32_t arr_index = 0;
    uint16_t VirtualAddress = 0;

    /* Transfer data from Page1 to Page0 */
    // Logic for trasferring data from old page to new page
    // Update the structure with the latest values.
    Oldpage = (OldPageID - 4);
    Blank_Check_Status = __asdk_flash_blank_check(OldPageID, 1);
    while (Blank_Check_Status)
    {

        VirtualAddress = (*(uint16_t *)OldPageID);
        (*(uint16_t *)(arr_for_page_transfer + VirtualAddress)) = (((*(uint32_t *)(OldPageID)) >> 16) & 0xFFFF);
        OldPageID = OldPageID + 4;
        Blank_Check_Status = __asdk_flash_blank_check(OldPageID, 1);
    }

    for (uint16_t i = 0; i < var_size; i++)
    {
        arr_for_page_transfer[addr_offset] = var_source[i];
        addr_offset = addr_offset + 1;
    }

    addr_offset = 0;

    while (size_of_var_to_transfer)
    {
        Data_To_Write = addr_offset + (((arr_for_page_transfer[arr_index + 1]) << 24) | ((arr_for_page_transfer[arr_index]) << 16));

        flash_write_config_t.destination_addr = (NewPageAddress + Iteration_Loop * 4);
        flash_write_config_t.source_addr = (uint32_t)(&Data_To_Write);
        flash_write_config_t.size_in_bytes = 4;

        flash_error_status = asdk_flash_write(&flash_write_config_t);
        size_of_var_to_transfer = size_of_var_to_transfer - 2;
        arr_index = arr_index + 2;
        addr_offset = addr_offset + 2;
        Iteration_Loop = Iteration_Loop + 1;
    }
    g_eeprom_variables.Destination_Address = flash_write_config_t.destination_addr + 4;
    /* Set new Page status to VALID_PAGE status */
    flash_write_config_t.destination_addr = NewPageAddress;
    g_eeprom_variables.Page_Status = VALID_PAGE;
    flash_write_config_t.source_addr =(uint32_t) (&g_eeprom_variables.Page_Status);
    /* Set the new Page status to VALID_PAGE status */
    flash_error_status = asdk_flash_write(&flash_write_config_t);

    /* Transfer process: transfer variables from old to the new active page */
    asdk_flash_erase_config_t flash_erase_config_t = {
        .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
        .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
        .Base_Sector_Address = Oldpage,
        .NbSectors = 8,
    };
    /* Erase OldPage */
    {
        flash_error_status = asdk_flash_erase_sector(&flash_erase_config_t);
    }

    if (ASDK_FLASH_STATUS_SUCCESS != flash_error_status)
    {
        FlashStatus = INITIAL_VALUE;
    }

    return FlashStatus;
}


/*!Initlialization function for the emulated eeprom module*/
uint32_t PageStatus0 = INVALID_PAGE;
uint32_t PageStatus1 = INVALID_PAGE;

asdk_errorcode_t asdk_emulated_eeprom_init(asdk_emulated_eeprom_init_deinit_config_t *eeeprom_init_config, uint8_t *global_structure_address, uint32_t global_structure_size)
{

    asdk_errorcode_t ret_value = ASDK_EMULATED_EEPROM_STATUS_SUCCESS;
    asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;
    cy_en_flashdrv_status_t Blank_Check_Status = CY_FLASH_DRV_SUCCESS;

    /*Variable declaration*/
    asdk_flash_init_deinit_config_t flash_init_config = {
        .flash_operation_mode = eeeprom_init_config->emulated_eeprom_operation_mode,
        .flash_type = eeeprom_init_config->flash_type_used,
    };

    /*Initialize the flash for eeprom emulation*/
    flash_error_t = asdk_flash_init(&flash_init_config);
    CY_ASSERT(flash_error_t == ASDK_FLASH_STATUS_SUCCESS);
    cy_en_flashdrv_status_t Page0_Blank_Check_Status = INITIAL_VALUE;
    cy_en_flashdrv_status_t Page1_Blank_Check_Status = INITIAL_VALUE;

    g_eeprom_variables.Var_Size_Backup = global_structure_size;
    g_eeprom_variables.structure_base_address_ptr = (uint32_t*)(global_structure_address);

    uint32_t Page0Address = (PAGE0_BASE_ADDRESS + 4);
    uint32_t Page1Address = (PAGE1_BASE_ADDRESS + 4);

    Blank_Check_Status = __asdk_flash_blank_check(PAGE0_BASE_ADDRESS, 1);
    if (CY_FLASH_DRV_SUCCESS == Blank_Check_Status)
    {
        PageStatus0 = ERASED;
    }
    else if (CY_FLASH_DRV_FLASH_NOT_ERASED == Blank_Check_Status)
    {
        // __disable_irq();
        // Cy_SysTick_Disable();
        PageStatus0 = (*(uint32_t *)PAGE0_BASE_ADDRESS);
        // Cy_SysTick_Enable();
        // __enable_irq();
    }

    Blank_Check_Status = __asdk_flash_blank_check(PAGE1_BASE_ADDRESS, 1);
    if (CY_FLASH_DRV_SUCCESS == Blank_Check_Status)
    {
        PageStatus1 = ERASED;
    }
    else if (CY_FLASH_DRV_FLASH_NOT_ERASED == Blank_Check_Status)
    {

        // __disable_irq();
        // Cy_SysTick_Disable();
        PageStatus1 = (*(uint32_t *)PAGE1_BASE_ADDRESS);
        // Cy_SysTick_Enable();
        // __enable_irq();
    }

    switch (PageStatus0)
    {
    case ERASED:
        if (VALID_PAGE == PageStatus1)
        {
            /*Copy the latest data from valid page to the structure passed as parameter*/
            uint16_t VirtualAddress = 0;
            Blank_Check_Status = __asdk_flash_blank_check(PAGE1_BASE_ADDRESS + 4, 1);
            while (Blank_Check_Status)
            {

                VirtualAddress = (*(uint16_t *)Page1Address);
                (*(uint16_t *)(global_structure_address + VirtualAddress)) = (((*(uint32_t *)(Page1Address)) >> 16) & 0xFFFF);
                Page1Address = Page1Address + 4;
                Blank_Check_Status = __asdk_flash_blank_check(Page1Address, 1);
            }

            g_eeprom_variables.Destination_Address = Page1Address;

            asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

            /* Erase Page0 */
            if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE0_BASE_ADDRESS))
            {
                asdk_flash_erase_config_t flash_erase_config_t = {
                    .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
                    .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
                    .Base_Sector_Address = PAGE0_BASE_ADDRESS,
                    .NbSectors = 8,
                };
                flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
                if (ASDK_FLASH_STATUS_SUCCESS != flash_error_t)
                {
                    break;
                }
            }
        }
        else /*First access of EEPROM (Page0 & Page1) or Invalid State and hence needs to be completely Erased or Formatted*/
        {
            __asdk_emulated_eeprom_format();
        }
        break;

    case VALID_PAGE:
        /*Both page cannot be in valid state so format the  eeprom */
        /*Erase both Page0 and Page1 and set Page0 as the Valid Page*/
        if (VALID_PAGE == PageStatus1)
        {
            /*Copy from the Latest VALID Page into SRAM
             Check the last address of both the pages
             If it is blank means its the active page so copy data from this page*/
            Page0_Blank_Check_Status = __asdk_flash_blank_check(PAGE0_END_ADDRESS - 4, 1);
            Page1_Blank_Check_Status = __asdk_flash_blank_check(PAGE1_END_ADDRESS - 4, 1);
            if (CY_FLASH_DRV_SUCCESS == Page0_Blank_Check_Status)
            {
                /*Copy the latest data from valid page to the structure passed as parameter*/
                uint16_t VirtualAddress = 0;
                Blank_Check_Status = __asdk_flash_blank_check(PAGE0_BASE_ADDRESS, 1);
                while (Blank_Check_Status)
                {

                    VirtualAddress = (*(uint16_t *)Page0Address);
                    (*(uint16_t *)(global_structure_address + VirtualAddress)) = (((*(uint32_t *)(Page0Address)) >> 16) & 0xFFFF);
                    Page0Address = Page0Address + 4;
                    Blank_Check_Status = __asdk_flash_blank_check(Page0Address, 1);
                }

                g_eeprom_variables.Destination_Address = Page0Address;

                asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

                /* Erase Page1 if not Erased */
                if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE1_BASE_ADDRESS))
                {
                    asdk_flash_erase_config_t flash_erase_config_t = {
                        .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
                        .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
                        .Base_Sector_Address = PAGE1_BASE_ADDRESS,
                        .NbSectors = 8,
                    };
                    flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
                    if (ASDK_FLASH_STATUS_SUCCESS != flash_error_t)
                    {
                        break;
                    }
                }
            }
            else if (CY_FLASH_DRV_SUCCESS == Page1_Blank_Check_Status)
            {
                /*Copy the latest data from valid page to the structure passed as parameter*/
                uint16_t VirtualAddress = 0;
                Blank_Check_Status = __asdk_flash_blank_check(PAGE1_BASE_ADDRESS + 4, 1);
                while (Blank_Check_Status)
                {

                    VirtualAddress = (*(uint16_t *)Page1Address);
                    (*(uint16_t *)(global_structure_address + VirtualAddress)) = (((*(uint32_t *)(Page1Address)) >> 16) & 0xFFFF);
                    Page1Address = Page1Address + 4;
                    Blank_Check_Status = __asdk_flash_blank_check(Page1Address, 1);
                }

                g_eeprom_variables.Destination_Address = Page1Address;

                asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

                /* Erase Page0 */
                if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE0_BASE_ADDRESS))
                {
                    asdk_flash_erase_config_t flash_erase_config_t = {
                        .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
                        .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
                        .Base_Sector_Address = PAGE0_BASE_ADDRESS,
                        .NbSectors = 8,
                    };
                    flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
                    if (ASDK_FLASH_STATUS_SUCCESS != flash_error_t)
                    {
                        break;
                    }
                }
            }
            else
            {
                __asdk_emulated_eeprom_format();
            }
        }
        else if (ERASED == PageStatus1)
        {
            /*Copy the latest data from valid page to the structure passed as parameter*/
            uint16_t VirtualAddress = 0;
            Blank_Check_Status = __asdk_flash_blank_check(PAGE0_BASE_ADDRESS + 4, 1);
            while (Blank_Check_Status)
            {

                VirtualAddress = (*(uint16_t *)Page0Address);
                (*(uint16_t *)(global_structure_address + VirtualAddress)) = (((*(uint32_t *)(Page0Address)) >> 16) & 0xFFFF);
                Page0Address = Page0Address + 4;
                Blank_Check_Status = __asdk_flash_blank_check(Page0Address, 1);
            }

            g_eeprom_variables.Destination_Address = Page0Address;

            asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

            /* Erase Page1 if not Erased */
            if (!__asdk_emulated_eeprom_verifypagefullyerased(PAGE1_BASE_ADDRESS))
            {
                asdk_flash_erase_config_t flash_erase_config_t = {
                    .EraseType = ASDK_FLASH_ERASETYPE_SECTOR_WISE,
                    .flash_operation_mode = ASDK_FLASH_OPERATION_BLOCKING_MODE,
                    .Base_Sector_Address = PAGE1_BASE_ADDRESS,
                    .NbSectors = 8,
                };
                flash_error_t = asdk_flash_erase_sector(&flash_erase_config_t);
                if (ASDK_FLASH_STATUS_SUCCESS != flash_error_t)
                {
                    break;
                }
            }
        }
        break;

    default:
        __asdk_emulated_eeprom_format();

        break;
    }

    return ret_value;
}

/*De-initialization function for the emulated eeprom module*/
asdk_errorcode_t asdk_emulated_eeprom_deinit(asdk_emulated_eeprom_init_deinit_config_t *eeeprom_deinit_config)
{
    asdk_errorcode_t ret_value = ASDK_EMULATED_EEPROM_STATUS_SUCCESS;
    asdk_errorcode_t flash_error_t = ASDK_FLASH_STATUS_SUCCESS;

    /*Variable declaration*/
    asdk_flash_init_deinit_config_t flash_deinit_config = {
        .flash_operation_mode = eeeprom_deinit_config->emulated_eeprom_operation_mode,
        .flash_type = eeeprom_deinit_config->flash_type_used,
    };

    (void)eeeprom_deinit_config->emulated_eeprom_size;

    /*De-initialize the flash for eeprom emulation*/
    flash_error_t = asdk_flash_deinit(&flash_deinit_config);

    if (ASDK_FLASH_STATUS_SUCCESS == flash_error_t)
    {
        ret_value = ASDK_EMULATED_EEPROM_STATUS_SUCCESS;
    }
    else
    {
        ret_value = ASDK_EMULATED_EEPROM_STATUS_ERROR;
    }

    return ret_value;
}

/*Emulated eeprom module update function*/
/*User can update the entire structure or a member of the structure using this function.*/
/*Alternate to the write function*/
asdk_errorcode_t asdk_emulated_eeprom_update(uint8_t *var_source, uint8_t *struct_source, uint32_t var_size, uint32_t struct_size)
{
    /* Local Variables to store EMULATED_EEPROM status */
    asdk_errorcode_t ret_value = ASDK_EMULATED_EEPROM_STATUS_SUCCESS;
    uint32_t addr_offset = INITIAL_VALUE_ZERO;
    uint16_t Status = INITIAL_VALUE_ZERO;
    addr_offset = var_source - struct_source;
    uint16_t addr_offset_backup = addr_offset;
    uint32_t var_size_backup = var_size;
    uint16_t Data_To_Write = INITIAL_VALUE_ZERO;
    uint32_t arr_index = INITIAL_VALUE_ZERO;

    /********************************************************************/
    /*Updating the Variable Size and Address Offset if not 2Bytes aligned*/
    var_size = var_size + (var_size % 2);

    // addr_offset = addr_offset - (addr_offset % 2);
    if (addr_offset_backup % 2 != 0)
    {
        var_size = var_size + (var_size % 2);
        if ((var_size_backup % 2 == 0))
        {
            var_size = var_size + 2;
        }
        uint8_t array_to_copy[var_size];
        if (var_size_backup % 2 == 0)
        {
            for (uint32_t i = 0; i < (var_size - 2); i++)
            {
                array_to_copy[i + 1] = var_source[i];
            }
            array_to_copy[var_size - 1] = struct_source[addr_offset + var_size - 1];
        }
        else
        {
            for (uint32_t i = 0; i < (var_size - 1); i++)
            {
                array_to_copy[i + 1] = var_source[i];
            }
        }
        array_to_copy[0] = struct_source[addr_offset];
        while (var_size)
        {
            Data_To_Write = (((array_to_copy[arr_index + 1]) << 8) | (array_to_copy[arr_index]));

            Status = __asdk_emulated_eeprom_verifypagefullwritevariable(addr_offset, Data_To_Write);

            var_size = var_size - 2;
            arr_index = arr_index + 2;
            addr_offset = addr_offset + 2;

            if (PAGE_FULL == Status)
            {
                break;
            }
        }

        if (Status == PAGE_FULL)
        {

            __asdk_emulated_eeprom_pagetransfer(array_to_copy, struct_source, var_size, struct_size);
        }
    }
    else
    {
        while (var_size)
        {
            Data_To_Write = (((var_source[arr_index + 1]) << 8) | (var_source[arr_index]));

            Status = __asdk_emulated_eeprom_verifypagefullwritevariable(addr_offset, Data_To_Write);

            var_size = var_size - 2;
            arr_index = arr_index + 2;
            addr_offset = addr_offset + 2;

            if (PAGE_FULL == Status)
            {
                break;
            }
        }

        if (Status == PAGE_FULL)
        {

            __asdk_emulated_eeprom_pagetransfer(var_source, struct_source, var_size, struct_size);
        }
    }

    return ret_value;
}

/*Emulated eeprom module write function*/
/*User can update the structure or a member of the structure using this function*/
asdk_errorcode_t asdk_emulated_eeprom_write(uint8_t *var_source, uint8_t *struct_source, uint32_t var_size, uint32_t struct_size)
{
    /* Local Variables to store EMULATED_EEPROM status */
    asdk_errorcode_t ret_value = ASDK_EMULATED_EEPROM_STATUS_SUCCESS;
    uint32_t addr_offset = INITIAL_VALUE_ZERO;
    uint16_t Status = INITIAL_VALUE_ZERO;
    addr_offset = var_source - struct_source;
    uint16_t Data_To_Write = INITIAL_VALUE_ZERO;
    int arr_index = INITIAL_VALUE_ZERO;

    /********************************************************************/
    /*Updating the Variable Size and Address Offset if not 2Bytes aligned*/
    var_size = var_size + (var_size % 2);
    addr_offset = addr_offset - (addr_offset % 2);

    /*******************************************************************/

    while (var_size)
    {

        // If address offset is not 2B aligned(not divisible by 2)
        if ((addr_offset % 2 != 0))
        {
            addr_offset = addr_offset - (addr_offset % 2);
            var_size = var_size + 1;
            uint8_t array_to_copy[var_size];

            /*Shift  the array by one element to right and write the base array with the struct array at addr_offset*/
            for (uint32_t i = 0; i < (var_size - 1); i++)
            {
                array_to_copy[i + 1] = var_source[i];
            }
            array_to_copy[0] = struct_source[addr_offset];
            Data_To_Write = (((array_to_copy[arr_index + 1]) << 8) | (array_to_copy[arr_index]));
        }
        else
        {
            Data_To_Write = (((var_source[arr_index + 1]) << 8) | (var_source[arr_index]));
        }

        Status = __asdk_emulated_eeprom_verifypagefullwritevariable(addr_offset, Data_To_Write);

        var_size = var_size - 2;
        arr_index = arr_index + 2;
        addr_offset = addr_offset + 2;

        if (PAGE_FULL == Status)
        {
            break;
        }
    }

    if (PAGE_FULL == Status)
    {

        __asdk_emulated_eeprom_pagetransfer(var_source, struct_source, var_size, struct_size);
    }

    return ret_value;
}

/*Emulated eeprom module read function.*/
/*User can read either the entire structure or a member of that using this function*/
asdk_errorcode_t asdk_emulated_eeprom_read(uint8_t *read_back_var, uint8_t *structure_member_addr, uint8_t *global_structure_addr, uint32_t structure_member_size)
{
    uint16_t validpage = PAGE0;
    uint32_t addressvalue = PAGE0_BASE_ADDRESS;
    uint16_t readstatus = INITIAL_VALUE;
    uint32_t address = EEPROM_START_ADDRESS, PageStartAddress = EEPROM_START_ADDRESS;
    uint16_t VirtualAddress = global_structure_addr - structure_member_addr;
    uint16_t read_back_index = INITIAL_VALUE_ZERO;
    cy_en_flashdrv_status_t Blank_Check_Status = INITIAL_VALUE;

    /* Get active Page for read operation */
    validpage = __asdk_emulated_eeprom_findvalidpage(READ_FROM_VALID_PAGE);

    /* Check if there is no valid page */
    if (validpage == NO_VALID_PAGE)
    {
        return NO_VALID_PAGE;
    }

    /* Get the valid Page start Address */
    PageStartAddress = (uint32_t)(EEPROM_START_ADDRESS + (uint32_t)(validpage * EMULATED_EEPROM_PAGE_SIZE));

    /*Get the address from which the valid variable sits*/
    addressvalue = PageStartAddress + 4;
    /* Get the valid Page end Address */
    address = (uint32_t)((EEPROM_START_ADDRESS - 2) + (uint32_t)((1 + validpage) * EMULATED_EEPROM_PAGE_SIZE));

    /* Check each active page address starting from end */
    while ((address > (PageStartAddress + 4)) && (0 < structure_member_size))
    {
        /* Get the current location content to be compared with virtual address */
        addressvalue = (*(volatile uint16_t *)address);

        Blank_Check_Status = __asdk_flash_blank_check(PageStartAddress, 1);
        if (false == Blank_Check_Status)
        {

            if (addressvalue == VirtualAddress)
            {
                (*(uint16_t *)(read_back_var + read_back_index)) = (((*(uint32_t *)(addressvalue)) >> 16) & 0xFFFF);
                Blank_Check_Status = __asdk_flash_blank_check(addressvalue, 1);
                addressvalue = addressvalue + 4;
                read_back_index = read_back_index + 2;
                structure_member_size = structure_member_size - 2;
            }
            else
            {
                addressvalue = addressvalue + 4;
            }
        }
    }

    /* Return readstatus value: (0: variable exist, 1: variable doesn't exist) */
    return readstatus;
}