##### TARGET core

SET(CC2745_CORE m33)

MESSAGE("CC2745_CORE: ${CC2745_CORE}")

##### CC2745 specific definitions

SET(CC2745_DEFS
    -DDeviceFamily_CC27XX
)

##### define compiler flags

set(CC2745_C_COMPILER_OPTIONS
    ${ASDK_COMMON_COMPILER_FLAGS}
    -Wall
    -mcpu=cortex-${CC2745_CORE}
    -mfloat-abi=hard
    -mfpu=fpv5-sp-d16
    -mlittle-endian
    -MP 
    -MF
    -MT
    -c
    # -v
)

set(CC2745_LINKER_OPTIONS
    ${ASDK_COMMON_COMPILER_FLAGS}
    -mcpu=cortex-m33 
    -mfloat-abi=hard
    -mfpu=fpv5-sp-d16 
    -mlittle-endian
    # -v
)

##### define linker flags

set(CC2745_APP_LINK_FLAGS
    ${CC2745_LINKER_OPTIONS}
    -Wl,-m=${APP_ELF_NAME}.map
    -Wl,-I${CMAKE_CURRENT_SOURCE_DIR}/sdk/source
    -Wl,-I${CMAKE_CURRENT_SOURCE_DIR}/generated_files
    -Wl,-I$ENV{ASDK_TIARMCLANG_TOOLCHAIN_ROOT}/$ENV{ASDK_TIARMCLANG_TOOLCHAIN_VERSION}/lib
    -Wl,--diag_wrap=off
    -Wl,--display_error_number
    -Wl,--warn_sections
    -Wl,--xml_link_info="${ARG_APP_ELF}_linkInfo.xml"
    -Wl,--rom_model
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/boards/lpf3/lpf3_freertos.cmd
)
