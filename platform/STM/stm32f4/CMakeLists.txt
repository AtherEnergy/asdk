MESSAGE("\nPlatform: STM32F4\n")

ENABLE_LANGUAGE(C ASM)

##### Top level includes

# INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/FindFlags.cmake)

##### create interface for SDK include paths compile options & definitions

## SDK includes

# SET(SDK_INC "")
# INC_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/sdk SDK_INC)

SET(SDK_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/BSP/STM32F4-Discovery
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/CMSIS/Include
)

## create interface to share include files & compiler options with DAL

ADD_LIBRARY(
    stm32f4_sdk_interface
    INTERFACE
)

target_compile_definitions(
    stm32f4_sdk_interface
    INTERFACE
        ${STM32F4_SDK_DEFS}
)

target_compile_options(
    stm32f4_sdk_interface
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${STM32F4_C_COMPILER_OPTIONS}>
        $<$<COMPILE_LANGUAGE:ASM>:${STM32F4_ASM_COMPILER_OPTIONS}>
)

#### Compile: platform = stm32f4_sdk.a + stm32f4_dal.a

## startup files

SET(SDK_ASM
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/gcc/startup_stm32f407xx.s
)

## system files

SET(SYS_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/sdk/Drivers/STM32F4xx_HAL_Driver/Src DRV_SRC)

## Filter driver template files

LIST(FILTER DRV_SRC EXCLUDE REGEX "stm32f4xx_hal_timebase_rtc_.*_template.c$")

## sdk configuration files

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/sdk_config/src SDK_CONFIG_SRC)

## Merge all source files as SDK_SRC_LIST

LIST(APPEND SDK_SRC_LIST
    ${SDK_CONFIG_SRC}
    ${SDK_ASM}
    ${SYS_SRC}
    ${DRV_SRC}
)

### Compile SDK as static library

## compile SDK

ADD_LIBRARY(stm32f4_sdk OBJECT ${SDK_SRC_LIST})

TARGET_INCLUDE_DIRECTORIES(
    stm32f4_sdk
    PRIVATE # hidden from application
        ${SDK_INC}
)

TARGET_LINK_LIBRARIES(
    stm32f4_sdk
    PRIVATE # hidden from application
        stm32f4_sdk_interface # use platform compile options
        # c   # libc
        # m   # libm used by SDK for clock related settings
)

### Compile DAL as static library

## Compile DAL

SET(
    DAL_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/dal/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dal/src DAL_SRC)

ADD_LIBRARY(stm32f4_dal OBJECT ${DAL_SRC})

ADD_DEPENDENCIES(stm32f4_dal stm32f4_sdk)

TARGET_INCLUDE_DIRECTORIES(
    stm32f4_dal
    PRIVATE # hidden from application
        ${DAL_INC}
        ${SDK_INC}
        ${ASDK_CORE_INC}
)

TARGET_LINK_LIBRARIES(
    stm32f4_dal
    PRIVATE # hidden from application
        stm32f4_sdk_interface # use platform compile options
        stm32f4_sdk
)

### Create platform

ADD_LIBRARY(platform INTERFACE)

TARGET_INCLUDE_DIRECTORIES(
    platform
    INTERFACE   # share with application
        ${DAL_INC}
        ${SDK_INC}
)

TARGET_LINK_OPTIONS(
    platform
    INTERFACE   # share with application
        ${STM32F4_GCC_LINKER_FLAGS}
)

TARGET_LINK_LIBRARIES(
    platform
    INTERFACE
        $<TARGET_OBJECTS:stm32f4_sdk>
        $<TARGET_OBJECTS:stm32f4_dal>
        stm32f4_sdk_interface # use platform compile options 
)
