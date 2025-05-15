##### TARGET core

IF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M0PLUS")
    SET(CYT2B75_CORE m0plus)
ELSE()
    SET(CYT2B75_CORE m4)
ENDIF()

MESSAGE("CYT2B75_CORE: ${CYT2B75_CORE}")

##### CYT2B75 specific definitions

SET(CYT2B75_DEFS
    -D_LINK_flash_
    -DCY_USE_PSVP=0
    -DSTARTER_KIT_REV3
    -DCY_MCU_rev_d
    -DCYT2B75BAE
    -Dtviibe1m
    -Dcyt2b7
    -D_CORE_c${CYT2B75_CORE}_
    -Dcyt2b75_c${CYT2B75_CORE}
)

##### define compiler flags

set(CYT2B75_C_COMPILER_OPTIONS
    ${ASDK_COMMON_COMPILER_FLAGS}
    -std=c99
    -Wall
    # -O
    # -g3
    -mcpu=cortex-${CYT2B75_CORE}
    -MD
    -MT
    -MF
    -Xlinker -print-memory-usage
)

IF(${CYT2B75_CORE} STREQUAL m4)
    set(${CYT2B75_C_COMPILER_OPTIONS} ${CYT2B75_C_COMPILER_OPTIONS} -mfloat-abi=soft)
ENDIF()

##### define assembler flags

set(CYT2B75_ASSEMBLER_OPTIONS
    ${CYT2B75_C_COMPILER_OPTIONS}
    -mthumb
)

##### define linker flags

set(CYT2B75_APP_LINK_FLAGS
    ${ASDK_COMMON_LINKER_FLAGS}
    ${CYT2B75_C_COMPILER_OPTIONS}
    -nostartfiles
    # --specs=nano.specs
    -Wl,-Map=${APP_ELF_NAME}.map
    -Wl,-e_start
)
