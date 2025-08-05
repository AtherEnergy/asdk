MESSAGE("\nPlatform: CC2745\n")

ENABLE_LANGUAGE(C ASM)

##### Top level includes

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/FindFlags.cmake)

##### create interface for SDK include paths compile options & definitions

## SDK includes

SET(SDK_INC "")

## add necessary SDK files
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/third_party/freertos/include)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/third_party/freertos/portable/GCC/ARM_CM33_NTZ/non_secure)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/posix/ticlang)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/devices)
LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk)

IF("${APP_USER_DEFINES}" STREQUAL "-Dfreertos")
    LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/freertos)
    LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/FreeRTOSConfig.h)
ELSEIF("${APP_USER_DEFINES}" STREQUAL "-Dnortos")
    LIST(APPEND SDK_INC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos)
    LIST(FILTER SDK_INC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/FreeRTOSConfig.h)
ENDIF()

SET(DRV_SRC "")
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/apps/Button.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/apps/LED.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/utils/List.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/utils/Math.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/utils/RingBuf.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/utils/StructRingBuf.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/ADC.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/adc/ADCLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/ADCBuf.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/adcbuf/ADCBufLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/apu/APULPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/batterymonitor/BatteryMonitorLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/batterymonitor/BatMonSupportLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/CAN.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/can/CANCC27XX.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/can/common/CANMCAN.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/third_party/mcan/MCAN.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/Comparator.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/comparator/ComparatorLPF3LP.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/dma/UDMALPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/GPIO.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/gpio/GPIOLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/I2C.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/i2c/I2CLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/I2CTarget.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/i2ctarget/I2CTargetLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/I2S.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/i2s/I2SLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/ITM.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/itm/ITMLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/NVS.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/nvs/NVSRAM.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/nvs/NVSSPI25X.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/PWM.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/pwm/PWMTimerLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/SD.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/sd/SDSPI.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/SDFatFS.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/SPI.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/spi/SPILPF3DMA.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/Temperature.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/temperature/TemperatureLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/timer/LGPTimerLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/UART2.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/uart2/UART2LPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/Watchdog.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/watchdog/WatchdogLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/nvs/NVSLPF3.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/power/PowerCC27XX.c)
LIST(APPEND DRV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/drivers/power/PowerCC27XX_lfoscCompensation.c)

FILE(GLOB_RECURSE SDK_CFG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/*.c)
FILE(GLOB_RECURSE DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/devices/cc27xx/driverlib/*.c)
LIST(FILTER DEV_SRC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk/source/ti/devices/cc27xx/driverlib/.*_veneers.c)

IF("${APP_USER_DEFINES}" STREQUAL "-Dfreertos")
    LIST(APPEND SDK_CFG_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/ti_freertos_config.c)
ELSEIF("${APP_USER_DEFINES}" STREQUAL "-Dnortos")
    LIST(FILTER SDK_CFG_SRC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/ti_freertos_config.c)
    LIST(FILTER SDK_CFG_SRC EXCLUDE REGEX ${CMAKE_CURRENT_SOURCE_DIR}/sdk_config_files/ti_freertos_portable_config.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/NoRTOS.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/posix_sleep.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/DebugP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/MutexP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/SemaphoreP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/SwiP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/SystemP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/ClockPLPF3_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/TimestampPLPF3_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/QueueP_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/HwiPCC27XX_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/dpl/PowerCC27XX_nortos.c)
    LIST(APPEND DEV_SRC ${CMAKE_CURRENT_SOURCE_DIR}/sdk/kernel/nortos/startup/startup_cc27xx_ticlang.c) 
ENDIF()

LIST(APPEND SRC_LIST
    ${SDK_CFG_SRC}
    ${DEV_SRC}
    ${DRV_SRC}
)

## create interface to share include files & compiler options with DAL

ADD_LIBRARY(
    cc2745_sdk_interface
    INTERFACE
)

target_compile_definitions(
    cc2745_sdk_interface
    INTERFACE
        ${CC2745_DEFS}
)

target_compile_options(
    cc2745_sdk_interface
    INTERFACE
        $<$<COMPILE_LANGUAGE:C>:${CC2745_C_COMPILER_OPTIONS}>
)

#### Compile: platform = cyt2b75_sdk.a + cyt2b75_dal.a

### Compile SDK as static library

### compile SDK

ADD_LIBRARY(cc2745_sdk INTERFACE)

TARGET_INCLUDE_DIRECTORIES(
    cc2745_sdk
    INTERFACE # hidden from application
        ${SDK_INC}
)

TARGET_SOURCES(
    cc2745_sdk
    INTERFACE
        ${SRC_LIST}
)

TARGET_LINK_LIBRARIES(
    cc2745_sdk
    INTERFACE # hidden from application
        cc2745_sdk_interface # use platform compile options
        -llibc.a)

### Compile DAL 

SET(
    DAL_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/dal/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../inc
)

MESSAGE("\n \n DAL_INC \n" ${DAL_INC})

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/dal/src DAL_SRC)

ADD_LIBRARY(cc2745_dal INTERFACE)

ADD_DEPENDENCIES(cc2745_dal cc2745_sdk)

TARGET_INCLUDE_DIRECTORIES(
    cc2745_dal
    INTERFACE # hidden from application
        ${DAL_INC}
        ${SDK_INC}
        ${ASDK_CORE_INC}
)

TARGET_SOURCES(
    cc2745_dal
    INTERFACE
        ${DAL_SRC}
)

TARGET_LINK_LIBRARIES(
    cc2745_dal
    INTERFACE # hidden from application
        cc2745_sdk_interface # use platform compile options
        cc2745_sdk
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
        ${CC2745_APP_LINK_FLAGS}
)

TARGET_LINK_LIBRARIES(
    platform
    INTERFACE
        cc2745_dal
        cc2745_sdk
        cc2745_sdk_interface # use platform compile options 
)
