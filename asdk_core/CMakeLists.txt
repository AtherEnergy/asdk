CMAKE_MINIMUM_REQUIRED(VERSION 3.24)

MESSAGE("CMake version:" ${CMAKE_VERSION})

SET(PROJECT_NAME "ASDK-GEN2")

### enable cross-compilation ###

SET(CMAKE_SYSTEM_NAME Generic)

### get parent directory ###

GET_FILENAME_COMPONENT(USER_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)

### set asdk & platform directory ###

SET(ASDK_DIR ${USER_ROOT_DIR}/asdk_core)
SET(PLATFORM_DIR ${USER_ROOT_DIR}/platform)
SET(MIDDLEWARE_DIR ${USER_ROOT_DIR}/catalyst)

### set asdk_core include directory ###

SET(ASDK_CORE_INC
    ${ASDK_DIR}/common
    ${ASDK_DIR}/inc
)

### top-level cmake includes ###

INCLUDE(${ASDK_DIR}/cmake/macro.cmake)
INCLUDE(${ASDK_DIR}/cmake/helper_functions.cmake)
INCLUDE(${PLATFORM_DIR}/platform.cmake)

### top-level project ###

PROJECT(${PROJECT_NAME})

### use c language

ENABLE_LANGUAGE(C ASM)

### build asdk = build platform, catalyst and RTOS

ASDK_GEN_VERSION_INFO()

# identify target platform

ADD_SUBDIRECTORY(${PLATFORM_DIR} ${CMAKE_CURRENT_BINARY_DIR}/platform)

# identify required catalyst modules

ADD_SUBDIRECTORY(${MIDDLEWARE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/catalyst)

# create asdk interface

ADD_LIBRARY(asdk INTERFACE)

ADD_DEPENDENCIES(asdk platform)
ADD_DEPENDENCIES(asdk catalyst)

# expose include directories of lower layers

target_include_directories(
    asdk
    INTERFACE 
        common
        platform
        catalyst
)

# expose link time symbols of lower layers

target_link_libraries(
    asdk
    INTERFACE
        platform
        catalyst
)
