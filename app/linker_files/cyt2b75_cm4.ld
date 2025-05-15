/* __DISCLAIMER_START__                                                      */
/******************************************************************************

* (c) 2014-2017, Cypress Semiconductor Corporation

* or a subsidiary of Cypress Semiconductor Corporation. All rights

* reserved.

*

* This software, including source code, documentation and related

* materials ( "Software" ), is owned by Cypress Semiconductor

* Corporation or one of its subsidiaries ( "Cypress" ) and is protected by

* and subject to worldwide patent protection (United States and foreign),

* United States copyright laws and international treaty provisions.

* Therefore, you may use this Software only as provided in the license

* agreement accompanying the software package from which you

* obtained this Software ( "EULA" ).

*

* If no EULA applies, Cypress hereby grants you a personal, nonexclusive,

* non-transferable license to copy, modify, and compile the

* Software source code solely for use in connection with Cypress' s

* integrated circuit products. Any reproduction, modification, translation,

* compilation, or representation of this Software except as specified

* above is prohibited without the express written permission of Cypress.

*

* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO

* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,

* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED

* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A

* PARTICULAR PURPOSE. Cypress reserves the right to make

* changes to the Software without notice. Cypress does not assume any

* liability arising out of the application or use of the Software or any

* product or circuit described in the Software. Cypress does not

* authorize its products for use in any products where a malfunction or

* failure of the Cypress product may reasonably be expected to result in

* significant property damage, injury or death ( "High Risk Product" ). By

* including Cypress' s product in a High Risk Product, the manufacturer

* of such system or application assumes all risk of such use and in doing

* so agrees to indemnify Cypress against all liability.

******************************************************************************/
/* __DISCLAIMER_END__                                                        */
/*****************************************************************************/
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

   This linker file needs to be preprocessed by GCC because LD does not support preprocessing.

   CMake build environment will take care of this!

   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/* Check that necessary definitions are available */
/*=======================================================================================

// Description of special symbols

//=======================================================================================

// sram_private_for_srom:

//     The SROM code needs an area for private data during system call processing. It will

//     not be accessible by user software in protection state NORMAL and beyond.

//     So it needs to be excluded from the total SRAM available to the user. The location of

//     this RAM depends on the MCU revision.

//

// sram_used_by_boot_base:

//     Cypress (ROM and Flash) boot firmware needs an area in SRAM (e.g. for stack). This may cause issues in case of

//     RAM build configurations when code/const sections are loaded there by the debugger, but may get overwritten

//     if reset is triggered by the debug script later on. So in case of RAM build configurations the linker should

//     check if the area of ROM type sections overlaps the boot area.

//

// sram_can_lin_boot_reserve:

//     Cypress CAN/LIN Bootloader is entered only if several conditions apply (refer Architecture TRM, "Flash Boot" chapter).

//     It will then use an area of SRAM for its data.

//     This may cause issues in case of RAM build configurations when code/const sections are loaded there by the debugger,

//     but may get overwritten if reset is triggered by the debug script later on. Therefore, this area shall be reserved.

//

// ecc_init_width:

//     Defines the most restrictive native ECC width (in bytes) of all "normal" memories (SRAM, DTCM, ITCM) in any

//     Traveo II derivate in order to keep the startup code dealing with ECC initialization generic

*/
/*=======================================================================================

// Following symbol and memory area definitions are device specific

//=======================================================================================*/
  /* Constants */
  /*===================================================================================

  //                              USER CONFIGURATION

  //===================================================================================*/
  /* Specify how much SRAM and CODE_FLASH shall be used by CM0+ application,

   * the rest of the memory will be automatically assigned to CM4 application */
  /*-----------------------------------------------------------------------------------*/
  /* Specify stack and heap sizes for CM0+ and CM4 application independently */
  /*===================================================================================*/
  /* Calculation of SRAM distribution among all cores */
  /* Calculation of CODE_FLASH distribution among all cores */
  /* Map core dependent definition to generic name for memory and section definition */
MEMORY
{
    SRAM (rwx) : ORIGIN = 0x08000000 + ((2K + 0) + (64K - (2K + 0))), LENGTH = ((128K - 0) - ((2K + 0) + (64K - (2K + 0))))
    CODE_FLASH (rx) : ORIGIN = 0x10000000 + (0 + 512K), LENGTH = (1088K - (0 + 512K))
    WORK_FLASH : ORIGIN = 0x14000000, LENGTH = 0x00018000 /*   96 KB */
    SFLASH : ORIGIN = 0x17000000, LENGTH = 0x00008000 /*   32 KB */
    SFLASH_ALT : ORIGIN = 0x17800000, LENGTH = 0x00008000 /*   32 KB */
}
/*=======================================================================================

//

// Program layout

*/
SECTIONS
{
    /*============================================================================================================

    // ROM type sections

    //------------------------------------------------------------------------------------------------------------*/
    /* Note: .intvec must be the first section in ROM in order for __cmx_vector_base_linker_symbol to be correctly calculated! */
    .intvec : ALIGN(1024 /* CPU specific vector table alignment requirement */)
    {
        *(.intvec)
    } >CODE_FLASH
    .text : ALIGN(4)
    {
        *(.text*)
    } >CODE_FLASH
    .rodata : ALIGN(4)
    {
        *(.rodata*)
    } >CODE_FLASH
    /* Special GCC ARM sections */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > CODE_FLASH
    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > CODE_FLASH
    /*============================================================================================================

    // RAM type sections

    //------------------------------------------------------------------------------------------------------------*/
    /* >>> SRAM <<< */
    /* In case of _LINK_sram_ the linker (LD) would set the LMA of '.data' to its VMA (because both end up in the same memory region).

     * Therefore, the '.data' section needs to be the first one of the "RAM type" sections so that the '__rom_type_sections_end_address' 

     * can be correctly calculated and startup code will not ECC initialize the RAM area occupied by the .data section (since its content

     * would have been loaded by the debugger already). In case of _LINK_flash the '.data' section can be arbitrarily placed in RAM. */
    .data : ALIGN_WITH_INPUT
    {
        . = ALIGN(8);
        *(.data*)
        . = ALIGN(8);
    } >SRAM AT>CODE_FLASH
    .bss (NOLOAD) : ALIGN(8)
    {
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
    } >SRAM
    .heap (NOLOAD) : ALIGN(8)
    {
        __heap_start = .;
        . += ALIGN(512, 8);
        __heap_end = .;
    } >SRAM
    .stack (NOLOAD) : ALIGN(8)
    {
        __stack_start = .;
        . += ALIGN((1K + (8 - 1)) & (~((8 - 1))) /* Ensure that stack size is an integer multiple of ECC init width (round up) */, 8);
        __stack_end = .;
    } >SRAM
    .intvec_ram (NOLOAD) : ALIGN(1024 /* CPU specific vector table alignment requirement */)
    {
        *(.intvec_ram)
    } >SRAM
    /* Linker symbols for startup code */
    __data_lma = LOADADDR(.data);
    __data_start = ADDR(.data);
    __data_end = ADDR(.data) + SIZEOF(.data);
    __bss_start = ADDR(.bss);
    __bss_end = ADDR(.bss) + SIZEOF(.bss);
    /*============================================================================================================

    // The start of CM4 vector table is required by CM0+ application to correctly

    // set CPUSS->CM4_VECTOR_TABLE_BASE register before releasing CM4 from reset

    // Following statements also ensure that the created symbol has the correct alignment

    //------------------------------------------------------------------------------------------------------------*/
    /*============================================================================================================

    // These symbols will be used by startup.c / startup_customize.h for RAM ECC initialisation

    //------------------------------------------------------------------------------------------------------------*/
    __ecc_init_sram_start_address = ORIGIN(SRAM);
    __ecc_init_sram_end_address = ORIGIN(SRAM) + LENGTH(SRAM) - 1;
    /*============================================================================================================

    // These special symbols mark the bounds of RAM and ROM memory.

    // They are used by the GHS MULTI debugger.

    //------------------------------------------------------------------------------------------------------------*/
    /*

    __ghs_romstart = ORIGIN(CODE_FLASH);

    __ghs_romend   = ORIGIN(SFLASH_ALT) + LENGTH(SFLASH_ALT);

    __ghs_ramstart = ORIGIN(SRAM);

    __ghs_ramend   = ORIGIN(SRAM) + LENGTH(SRAM);

    */
}
