SET(ASDK_ARM_TOOLCHAIN_BIN $ENV{ASDK_ARM_TOOLCHAIN_ROOT}/$ENV{ASDK_ARM_TOOLCHAIN_VERSION}/bin)

IF(WIN32)
    string(REPLACE "\\" "/" ASDK_ARM_TOOLCHAIN_BIN ${ASDK_ARM_TOOLCHAIN_BIN})
ENDIF()

SET(ARM_CROSS_COMPILER arm-none-eabi-)
SET(CMAKE_COMPILER_PATH ${ASDK_ARM_TOOLCHAIN_BIN}/${ARM_CROSS_COMPILER})
SET(CMAKE_C_COMPILER   ${ASDK_ARM_TOOLCHAIN_BIN}/${ARM_CROSS_COMPILER}gcc)
SET(CMAKE_CXX_COMPILER ${ASDK_ARM_TOOLCHAIN_BIN}/${ARM_CROSS_COMPILER}g++)

IF(WIN32)
    SET(CMAKE_C_COMPILER   ${CMAKE_C_COMPILER}.exe)
    SET(CMAKE_CXX_COMPILER ${CMAKE_CXX_COMPILER}.exe)
ENDIF()

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  ${ASDK_ARM_TOOLCHAIN_BIN})

#set(CMAKE_EXE_LINKER_FLAGS "-specs=nano.specs --specs=nosys.specs --sysroot=/opt/gcc-arm-none-eabi/gcc-arm-none-eabi-7-2018-q2-update/arm-none-eabi" CACHE INTERNAL "")
set(CMAKE_EXE_LINKER_FLAGS " --specs=nosys.specs " CACHE INTERNAL "")
SET(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_VERBOSE_MAKEFILE ON)
# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# Adding the CPU Architecture definition macro, this can be used to place platform specific actions 
add_compile_definitions(MCU_ARCH_ARM)