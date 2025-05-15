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
    asdk_flash.c

    @path
    platform/Infineon/cyt2b75/dal/src/asdk_flash.c

    @Created on
    Sep 10, 2023

    @Author
    Ather Energy Limited

    @Copyright
    Copyright (c) Ather Energy Limited. All rights reserved.

    @brief
    This file implements the Flash module for CYT2B75 microcontroller.

*/

/*==============================================================================

                           INCLUDE FILES

==============================================================================*/

// dal includes
#include "asdk_mcu_pins.h" // Defines MCU pins as is
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
static bool g_flashoperation_complete_flag = false;
static bool g_nb_modeenabled = false;
cy_un_flash_context_t sromContext = {0};

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : MACROS

==============================================================================*/
#define FLASH_USER_MARGIN 0x1U
#define FLASH_FACTORY_MARGIN 0x2U
#define CODE_LARGE_SECTOR_SIZE_CYT2B7 CY_CODE_LES_SIZE_IN_BYTE // 32KB
#define CODE_SMALL_SECTOR_SIZE_CYT2B7 CY_CODE_SES_SIZE_IN_BYTE // 8KB
#define WORK_LARGE_SECTOR_SIZE_CYT2B7 CY_WORK_LES_SIZE_IN_BYTE // 2KB
#define WORK_SMALL_SECTOR_SIZE_CYT2B7 CY_WORK_SES_SIZE_IN_BYTE // 128B
#define SECTOR_SIZE_142 0x800                                  // Erase only 2kB at a time
#define DATA_WRITE_SIZE 8

/*Work Flash Memory Partitioning*/
#define WORKFLASH_LARGE_START_ADDRESS 0x14000000
#define WORKFLASH_LARGE_END_ADDRESS 0x14011fff
#define WORKFLASH_SMALL_START_ADDRESS 0x14012000
#define WORKFLASH_SMALL_END_ADDRESS 0x14017FFF

/*Code Flash Memory Partitioning*/
#define CODE_LARGE_START_ADDR 0x10000000
#define CODE_LARGE_END_ADDR 0x100EFFFF
#define CODE_SMALL_START_ADDR 0x100F0000
#define CODE_SMALL_END_ADDR 0x1010FFFF

#define FLASH_SECTOR_SIZE CODE_LARGE_SECTOR_SIZE_CYT2B7
#define FLASH_SECTOR_SIZE_D WORK_LARGE_SECTOR_SIZE_CYT2B7

/*==============================================================================

                      LOCAL DEFINITIONS AND TYPES : ENUMS

==============================================================================*/

/*==============================================================================

                            LOCAL FUNCTION PROTOTYPES

==============================================================================*/
asdk_flash_callback_fun_t user_flash_callback_function;
static inline bool __asdk_workflash_blank_check(asdk_flash_operation_config_t *flash_read_config);

/*******************************************************************************
 * Function __asdk_workflash_blank_check
 ****************************************************************************/
/**
 *
 * This function is used to check if the work flash address is in erased state or not
 * This function will be internal to DAL of flash and is very much cyt specific
 *
 *******************************************************************************/
static inline bool __asdk_workflash_blank_check(asdk_flash_operation_config_t *flash_read_config)
{

  uint32_t status;
  CY_ASSERT(Cy_Flash_WorkBoundsCheck(flash_read_config->source_addr) == CY_FLASH_IN_BOUNDS);

  if (flash_read_config->flash_operation_mode == ASDK_FLASH_OPERATION_NON_BLOCKING_MODE)
  {
    CY_ASSERT(g_nb_modeenabled == true);

    /* Only for non-blocking operation */
    g_flashoperation_complete_flag = false;
  }

  cy_stc_flash_blankcheck_config_t blankCheckConfig;
  blankCheckConfig.addrToBeChecked = (uint32_t *)flash_read_config->source_addr;
  // blankCheckConfig.numOfWordsToBeChecked = Cy_Flash_IsWorkSmallSector(srcaddr)
  //                                              ? CY_WORK_SES_SIZE_IN_WORD
  //                                              : CY_WORK_LES_SIZE_IN_WORD;
  blankCheckConfig.numOfWordsToBeChecked = flash_read_config->size_in_bytes;

  status = Cy_Flash_BlankCheck(NULL, &blankCheckConfig, flash_read_config->flash_operation_mode);
  if (status == CY_FLASH_DRV_SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  };
}
/*******************************************************************************
 * Function asdk_flashHandler
 ****************************************************************************/
/**
 *
 * This is interrupt handler called when SROM API was finished by CM0+ via IPC
 * This function will be assigned in "asdk_flash_init".
 *
 *******************************************************************************/
static void asdk_flashHandler(void)
{
  un_srom_api_resps_t apiResp;
  cy_en_srom_api_status_t sromDrvStatus = Cy_Srom_GetApiResponse(&apiResp);
  if (sromDrvStatus != CY_SROM_STATUS_SUCCESS)
  {
    CY_ASSERT(false);
  }

  g_flashoperation_complete_flag = true;
  // Call the user callback function
  user_flash_callback_function();
  Cy_Flashc_InvalidateFlashCacheBuffer();
}

/*A function for user to know whether CM0+ completed erase/program flash or not*/
bool asdk_Is_SROM_API_Completed()
{
  bool api_status = false;
  api_status = g_flashoperation_complete_flag;
  return api_status;
}

/*Initlialization function for the Flash*/
asdk_errorcode_t asdk_flash_init(asdk_flash_init_deinit_config_t *flash_config)
{

  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_config->flash_operation_mode)
  {
    /*********************Setting for IPCs*****************************/
    Cy_Srom_SetResponseHandler(asdk_flashHandler, CPUIntIdx3_IRQn);
    NVIC_SetPriority(CPUIntIdx3_IRQn, 3ul);
    NVIC_EnableIRQ(CPUIntIdx3_IRQn);
    g_nb_modeenabled = true;
  }
  else
  {
    g_nb_modeenabled = false;
  }

  (void)g_nb_modeenabled; // avoid "unused" copiler warning if NDEBUG is set
  if (ASDK_FLASH_INIT_FLASHTYPE_BOTH_CODE_FLASH_DATA_FLASH == flash_config->flash_type)
  {
    /*  Flash Write Enable   */
    Cy_Flashc_MainWriteEnable();
    Cy_Flashc_WorkWriteEnable();
  }
  else if (ASDK_FLASH_INIT_FLASHTYPE_CODE_FLASH == flash_config->flash_type)
  {
    Cy_Flashc_MainWriteEnable();
  }
  else if (ASDK_FLASH_INIT_FLASHTYPE_DATA_FLASH == flash_config->flash_type)
  {
    Cy_Flashc_WorkWriteEnable();
  }
  else
  {
    return ASDK_FLASH_ERROR_INVALID_FLASH_TYPE;
  }

  g_flashoperation_complete_flag = true;

  return ret_value;
}

/*De-initialization function for the Flash*/
asdk_errorcode_t asdk_flash_deinit(asdk_flash_init_deinit_config_t *flash_config)
{

  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;

  if (ASDK_FLASH_INIT_FLASHTYPE_BOTH_CODE_FLASH_DATA_FLASH == flash_config->flash_type)
  {
    /*  Flash Write Enable   */
    Cy_Flashc_MainWriteDisable();
    Cy_Flashc_WorkWriteDisable();
  }
  else if (ASDK_FLASH_INIT_FLASHTYPE_CODE_FLASH == flash_config->flash_type)
  {
    Cy_Flashc_MainWriteDisable();
  }
  else if (ASDK_FLASH_INIT_FLASHTYPE_DATA_FLASH == flash_config->flash_type)
  {
    Cy_Flashc_WorkWriteDisable();
  }
  else
  {
    return ASDK_FLASH_ERROR_INVALID_FLASH_TYPE;
  }
  return ret_value;
}



/*!Flash read function*/
asdk_errorcode_t asdk_flash_read(asdk_flash_operation_config_t *flash_read_config)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;
  // Checking for the valid work flash address for reading
  if ((CODE_LARGE_START_ADDR <= (flash_read_config->source_addr)) && ((CODE_SMALL_END_ADDR) >= (flash_read_config->source_addr)))
  {

    memcpy((void *)flash_read_config->destination_addr, (void *)flash_read_config->source_addr, flash_read_config->size_in_bytes);
  }
  // Checking for the valid code flash address for reading
  else if ((WORKFLASH_LARGE_START_ADDRESS <= (flash_read_config->source_addr)) && ((WORKFLASH_SMALL_END_ADDRESS) >= (flash_read_config->source_addr)))
  {
    if (false == (__asdk_workflash_blank_check(flash_read_config)))
    {
      memcpy((void *)flash_read_config->destination_addr, (void *)flash_read_config->source_addr, flash_read_config->size_in_bytes);
    }
  }
  else
  {
    return ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS;
  }

  return ret_value;
}

/*!Flash write function*/
asdk_errorcode_t asdk_flash_write(asdk_flash_operation_config_t *flash_write_config)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;
  uint32_t totalSize = 512;
  uint8_t writeBuffer[512];
  cy_en_flashdrv_status_t ProgramFlashStatus = CY_FLASH_DRV_SUCCESS;
  cy_stc_flash_programrow_config_t programRowConfig =
      {
          .blocking = CY_FLASH_PROGRAMROW_BLOCKING,
          .skipBC = CY_FLASH_PROGRAMROW_SKIP_BLANK_CHECK,
          .dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_64BIT,
          .dataLoc = CY_FLASH_PROGRAMROW_DATA_LOCATION_SRAM,
          .intrMask = CY_FLASH_PROGRAMROW_NOT_SET_INTR_MASK,
          .destAddr = NULL,
          .dataAddr = NULL,
      };

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_write_config->flash_operation_mode)
  {
    CY_ASSERT(g_nb_modeenabled == true);
    g_flashoperation_complete_flag = false;
  }

  if (NULL == flash_write_config)
  {
    return ASDK_FLASH_ERROR_NULL_PTR;
  }

  uint32_t addressToBeWritten = (uint32_t)flash_write_config->destination_addr;
  totalSize = flash_write_config->size_in_bytes;
  uint32_t addressOfSource = (uint32_t)flash_write_config->source_addr;

  // while (/*(totalSize != 0) && (CY_FLASH_DRV_SUCCESS != ProgramFlashStatus)*/true)
  while ((totalSize != 0) && (CY_FLASH_DRV_SUCCESS == ProgramFlashStatus))

  {

    uint16_t dataSizeToBeWritten = 0;
    /*Address Validation checking*/
    if (!((CODE_LARGE_START_ADDR <= addressToBeWritten) && ((CODE_SMALL_END_ADDR) >= (addressToBeWritten + totalSize))))
    {

      if (!((WORKFLASH_LARGE_START_ADDRESS <= addressToBeWritten) && ((WORKFLASH_SMALL_END_ADDRESS) >= (addressToBeWritten + totalSize))))
      {
        return ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS;
      }
    }

    if (((WORKFLASH_LARGE_START_ADDRESS <= addressToBeWritten) && ((WORKFLASH_SMALL_END_ADDRESS) >= (addressToBeWritten + totalSize))))
    {

      programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_32BIT;
      dataSizeToBeWritten = 4;
      if (totalSize >= 4)
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_32BIT;
        dataSizeToBeWritten = 4;
      }
      else
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_32BIT;
        dataSizeToBeWritten = totalSize;
        for (uint8_t i = totalSize; i < 4; i++)
        {
          // Padding with 0xFF
          writeBuffer[i] = 0xFF;
        }
      }
    }
    else
    {

      if (totalSize >= 512)
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_4096BIT;
        dataSizeToBeWritten = 512;
      }
      else if (totalSize >= 32)
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_256BIT;
        dataSizeToBeWritten = 32;
      }
      else if (totalSize >= 8)
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_64BIT;
        dataSizeToBeWritten = 8;
      }
      else
      {
        programRowConfig.dataSize = CY_FLASH_PROGRAMROW_DATA_SIZE_64BIT;
        dataSizeToBeWritten = totalSize;
        for (uint8_t i = totalSize; i < 8; i++)
        {
          // Padding with 0xFF
          writeBuffer[i] = 0xFF;
        }
      }
    }

    memcpy(writeBuffer, (void *)addressOfSource, dataSizeToBeWritten);
    programRowConfig.destAddr = (uint32_t *)addressToBeWritten;
    programRowConfig.dataAddr = (uint32_t *)writeBuffer;

    if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_write_config)
    {
      programRowConfig.blocking = CY_FLASH_PROGRAMROW_NON_BLOCKING;
    }
    ProgramFlashStatus = Cy_Flash_ProgramRow(&sromContext, &programRowConfig, CY_FLASH_DRIVER_BLOCKING);
    totalSize = totalSize - dataSizeToBeWritten;
    addressToBeWritten = addressToBeWritten + dataSizeToBeWritten;
    addressOfSource = addressOfSource + dataSizeToBeWritten;
  }

  return ret_value;
}


/*!Flash erase the entire flash function*/
asdk_errorcode_t asdk_flash_erase_all_block(asdk_flash_erase_config_t *flash_erase_config)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;
  cy_en_flash_programrow_blocking_t erase_mode = CY_FLASH_PROGRAMROW_NON_BLOCKING;

  cy_en_flashdrv_status_t flash_erase_status = CY_FLASH_DRV_SUCCESS;

  if (NULL == flash_erase_config)
  {
    return ASDK_FLASH_ERROR_NULL_PTR;
  }

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    CY_ASSERT(g_nb_modeenabled == true);
    g_flashoperation_complete_flag = false;
  }

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    erase_mode = CY_FLASH_PROGRAMROW_NON_BLOCKING;
  }
  else if (ASDK_FLASH_OPERATION_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    erase_mode = CY_FLASH_PROGRAMROW_BLOCKING;
  }
  else
  {
    ret_value = ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE;
  }

  flash_erase_status = Cy_Flash_EraseAll(&sromContext, erase_mode);

  if (CY_FLASH_DRV_SUCCESS != flash_erase_status)
  {
    ret_value = ASDK_FLASH_STATUS_ERROR;
  }

  return ret_value;
}

/*!Flash erase sectorwise function*/
asdk_errorcode_t asdk_flash_erase_sector(asdk_flash_erase_config_t *flash_erase_config)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;
  cy_en_flashdrv_status_t flash_sector_erase_status = CY_FLASH_DRV_SUCCESS;
  cy_en_flash_programrow_blocking_t erase_mode = CY_FLASH_PROGRAMROW_NON_BLOCKING;
  cy_stc_flash_erasesector_config_t eraseSectorConfig = {0};

  uint32_t sector_cnt = flash_erase_config->NbSectors;
  uint32_t sector_size;
  uint8_t index;

  uint32_t base_start = flash_erase_config->Base_Sector_Address;
  if (NULL == flash_erase_config)
  {
    return ASDK_FLASH_ERROR_NULL_PTR;
  }

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    CY_ASSERT(g_nb_modeenabled == true);
    g_flashoperation_complete_flag = false;
  }

  if (ASDK_FLASH_OPERATION_NON_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    erase_mode = CY_FLASH_PROGRAMROW_NON_BLOCKING;
  }
  else if (ASDK_FLASH_OPERATION_BLOCKING_MODE == flash_erase_config->flash_operation_mode)
  {
    erase_mode = CY_FLASH_PROGRAMROW_BLOCKING;
  }
  else
  {
    ret_value = ASDK_FLASH_ERROR_INVALID_FLASH_OPERATION_MODE;
  }

  // Erase work flash and verify
  eraseSectorConfig.Addr = NULL;
  eraseSectorConfig.blocking = erase_mode;
  eraseSectorConfig.intrMask = CY_FLASH_ERASESECTOR_NOT_SET_INTR_MASK;

  for (index = 0; index < sector_cnt; index++)
  {
    // Need to check if the Small Sectors will also be used for the flash programming
    if ((CODE_LARGE_START_ADDR <= base_start) && ((CODE_LARGE_END_ADDR) >= (base_start)))
    {

      sector_size = CODE_LARGE_SECTOR_SIZE_CYT2B7;
    }
    else if ((CODE_SMALL_START_ADDR <= base_start) && ((CODE_SMALL_END_ADDR) >= (base_start)))
    {

      sector_size = CODE_SMALL_SECTOR_SIZE_CYT2B7;
    }
    // Need to check if the Small Sectors will also be used for the wflash programming
    else if ((WORKFLASH_LARGE_START_ADDRESS <= base_start) && ((WORKFLASH_LARGE_END_ADDRESS) >= (base_start)))
    {

      sector_size = WORK_LARGE_SECTOR_SIZE_CYT2B7;
    }
    else if ((WORKFLASH_SMALL_START_ADDRESS <= base_start) && ((WORKFLASH_SMALL_END_ADDRESS) >= (base_start)))
    {

      sector_size = WORK_SMALL_SECTOR_SIZE_CYT2B7;
    }
    else
    {
      return ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS;
    }

    eraseSectorConfig.Addr = (uint32_t *)(base_start);
    flash_sector_erase_status = Cy_Flash_EraseSector(NULL, &eraseSectorConfig, erase_mode);
    base_start += sector_size;
    if (CY_FLASH_DRV_SUCCESS != flash_sector_erase_status)
    {
      ret_value = ASDK_FLASH_STATUS_ERROR;
    }
  }

  return ret_value;
}

/*!Flash get the sectorsize for the passed address*/
asdk_errorcode_t asdk_flash_get_sector_size(uint32_t address, uint32_t *flash_sector_size)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;

  // Need to check if the Small Sectors will also be used for the flash programming
  if ((CODE_LARGE_START_ADDR <= address) && ((CODE_LARGE_END_ADDR) >= (address)))
  {

    *flash_sector_size = CODE_LARGE_SECTOR_SIZE_CYT2B7;
  }
  else if ((CODE_SMALL_START_ADDR <= address) && ((CODE_SMALL_END_ADDR) >= (address)))
  {

    *flash_sector_size = CODE_SMALL_SECTOR_SIZE_CYT2B7;
  }
  // Need to check if the Small Sectors will also be used for the wflash programming
  else if ((WORKFLASH_LARGE_START_ADDRESS <= address) && ((WORKFLASH_LARGE_END_ADDRESS) >= (address)))
  {

    *flash_sector_size = WORK_LARGE_SECTOR_SIZE_CYT2B7;
  }
  else if ((WORKFLASH_SMALL_START_ADDRESS <= address) && ((WORKFLASH_SMALL_END_ADDRESS) >= (address)))
  {

    *flash_sector_size = WORK_SMALL_SECTOR_SIZE_CYT2B7;
  }
  else
  {
    return ASDK_FLASH_ERROR_INVALID_FLASH_ADDRESS;
  }

  return ret_value;
}


/*Register/Install the callback function for the flash incase of non-blocking mode */
asdk_errorcode_t asdk_flash_install_callback(asdk_flash_callback_fun_t callback_fun)
{
  asdk_errorcode_t ret_value = ASDK_FLASH_STATUS_SUCCESS;

  user_flash_callback_function = callback_fun;

  return ret_value;
}
