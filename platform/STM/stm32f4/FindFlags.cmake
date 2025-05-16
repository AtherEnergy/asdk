##### STM32F4 specific definitions

SET(STM32F4_SDK_DEFS
    ${ASDK_COMMON_DEFINITIONS}
    -DUSE_HAL_DRIVER
    -DUSE_STM32F4_DISCO
    -DSTM32F407xx
)

##### define compiler flags

SET(STM32F4_C_COMPILER_OPTIONS
    ${ASDK_COMMON_COMPILER_FLAGS}
    --specs=nano.specs
    -std=gnu11
    -Wall
    -mcpu=cortex-m4
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
    -mthumb
    -fstack-usage
    -Xlinker -print-memory-usage
)

##### define assembler flags

SET(STM32F4_ASM_COMPILER_OPTIONS
    ${STM32F4_C_COMPILER_OPTIONS}
    -c
    -x assembler-with-cpp
)

##### define linker flags

SET(STM32F4_GCC_LINKER_FLAGS
    ${ASDK_COMMON_LINKER_FLAGS}
    ${STM32F4_C_COMPILER_OPTIONS}
    -static
    # --specs=nosys.specs
    -Wl,-Map=${APP_ELF_NAME}.map
    -Wl,--start-group
    -lc
    -lm
    -Wl,--end-group
)
