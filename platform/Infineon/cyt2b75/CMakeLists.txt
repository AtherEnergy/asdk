MESSAGE("\nPlatform: CYT2B75\n")

ENABLE_LANGUAGE(C ASM)

##### Top level includes

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/FindFlags.cmake)

##### create interface for SDK include paths compile options & definitions

## SDK includes

SET(SDK_INC "")
INC_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/sdk SDK_INC)

## resolve include statements like '#include "syslib/syslib.h"' for SDK
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/*)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/mw/*)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/src/drivers/*)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/mw/)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/src/drivers/)

## remove un-used files
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/cxpi)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/smif)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/hdr/rev_b)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/hdr/rev_c)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe2m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe4m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe512k)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibh4m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibh8m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibh16m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviic2d4m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviic2d6m)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviic2d4mddr)
LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviice4m)

## create interface to share include files & compiler options with DAL

ADD_LIBRARY(
    cyt2b75_sdk_interface
    INTERFACE
)

target_compile_definitions(
    cyt2b75_sdk_interface
    INTERFACE
        ${CYT2B75_DEFS}
)

target_compile_options(
    cyt2b75_sdk_interface
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${CYT2B75_C_COMPILER_OPTIONS}>
        $<$<COMPILE_LANGUAGE:ASM>:${CYT2B75_ASSEMBLER_OPTIONS}>
)

#### Compile: platform = cyt2b75_sdk.a + cyt2b75_dal.a

## startup files
SET(
    SDK_ASM
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/syslib/gcc/cy_syslib_gcc.S
)

IF(USE_RTOS)
    IF(EXISTS "${CMAKE_SOURCE_DIR}/asdk-gen2/middleware/rtos/RTOS_uC-OS3")
        LIST(
            APPEND SDK_ASM
            ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/startup/gcc/RTOS_uC-OS3/startup_c${CYT2B75_CORE}.S
        )
    ENDIF()
ELSE()
    LIST(
        APPEND SDK_ASM
        ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/startup/gcc/startup_c${CYT2B75_CORE}.S
    )
ENDIF()

## system files

FILE(GLOB_RECURSE SYS_STARTUP_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/startup/*.c)
FILE(GLOB_RECURSE SYS_INT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/src/interrupts/*.c)

SET(
    SYS_SRC
    ${SYS_STARTUP_SRC}
    ${SYS_INT_SRC}
    ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/src/system/rev_d/system_tviibe1m_c${CYT2B75_CORE}.c
)

## driver files

FILE(GLOB_RECURSE COMMON_DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/*.c)
FILE(GLOB_RECURSE PLATFORM_DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/tviibe1m/src/drivers/*.c)

## remove unused drivers
LIST(FILTER COMMON_DRV_SRC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/cxpi)
LIST(FILTER COMMON_DRV_SRC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/common/src/drivers/smif)

SET(
    DRV_SRC
    ${COMMON_DRV_SRC}
    ${PLATFORM_DRV_SRC}
)

## Merge all source files as SDK_SRC_LIST

LIST(APPEND SDK_SRC_LIST
    ${SDK_ASM}
    ${SYS_SRC}
    ${DRV_SRC}
)

### Compile SDK as static library

### compile SDK

ADD_LIBRARY(cyt2b75_sdk STATIC ${SDK_SRC_LIST})

TARGET_INCLUDE_DIRECTORIES(
    cyt2b75_sdk
    PRIVATE # hidden from application
        ${SDK_INC}
)

TARGET_LINK_LIBRARIES(
    cyt2b75_sdk
    PRIVATE # hidden from application
        cyt2b75_sdk_interface # use platform compile options
        c   # libc
        m   # libm used by SDK for clock related settings
)

### Compile DAL 

SET(
    DAL_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/dal/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dal/src DAL_SRC)

ADD_LIBRARY(cyt2b75_dal STATIC ${DAL_SRC})

ADD_DEPENDENCIES(cyt2b75_dal cyt2b75_sdk)

TARGET_INCLUDE_DIRECTORIES(
    cyt2b75_dal
    PRIVATE # hidden from application
        ${DAL_INC}
        ${SDK_INC}
        ${ASDK_CORE_INC}
)

TARGET_LINK_LIBRARIES(
    cyt2b75_dal
    PRIVATE # hidden from application
        cyt2b75_sdk_interface # use platform compile options
        cyt2b75_sdk
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
        ${CYT2B75_APP_LINK_FLAGS}
)

TARGET_LINK_LIBRARIES(
    platform
    INTERFACE
        cyt2b75_dal
        cyt2b75_sdk
        cyt2b75_sdk_interface # use platform compile options 
)
