# generate version file
function(ASDK_GEN_VERSION_INFO)

    cmake_host_system_information(RESULT HOSTNAME QUERY HOSTNAME)

    STRING(TIMESTAMP DATE "%Y-%m-%d")
    STRING(TIMESTAMP TIME "%H:%M:%S")

    file(READ ${CMAKE_CURRENT_LIST_DIR}/.version VER)
    string(REGEX REPLACE "\n$" "" VER "${VER}")
    string(REPLACE "." ";" VERSION_LIST ${VER})
    list(GET VERSION_LIST 0 ASDK_MAJOR_VERSION)
    list(GET VERSION_LIST 1 ASDK_MINOR_VERSION)
    list(GET VERSION_LIST 2 ASDK_PATCH_VERSION)

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    execute_process(
        COMMAND git -C ../ rev-parse --short HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE APP_GIT_COMMIT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND git -C ../ rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        OUTPUT_VARIABLE APP_GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    IF(WIN32)
        SET(USERNAME $ENV{USERNAME})
    ELSE()
        SET(USERNAME $ENV{USER})
    ENDIF()

    IF(${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        SET(BUILD_TYPE 1) # debug
    ELSE()
        SET(BUILD_TYPE 0) # release
    ENDIF()

    file(WRITE ${PROJECT_BINARY_DIR}/asdk_version.h.in
    "/* ###################################################################\n"
    "**\n"
    "**     This file was created by CMAKE BUILD\n"
    "**     for the ${TARGET_PLATFORM} series of microcontrollers.\n"
    "**\n"
    "** ################################################################### */ \n"
    "\n"
    "\#define ASDK_MAJOR_VERSION @ASDK_MAJOR_VERSION@ \n"
    "\#define ASDK_MINOR_VERSION @ASDK_MINOR_VERSION@ \n"
    "\#define ASDK_PATCH_VERSION @ASDK_PATCH_VERSION@ \n"
    "\#define ASDK_GIT_BRANCH \"@GIT_BRANCH@\"\n"
    "\#define ASDK_GIT_COMMIT \"@GIT_COMMIT@\"\n"
    "\#define ASDK_BUILD_DATE \"@DATE@\"\n"
    "\#define ASDK_BUILD_TIME \"@TIME@\"\n"
    "\#define ASDK_EXE_NAME \"@EXE_NAME@\"\n"
    "\#define APP_MAJOR_VERSION @APP_MAJOR_VERSION@ \n"
    "\#define APP_MINOR_VERSION @APP_MINOR_VERSION@ \n"
    "\#define APP_PATCH_VERSION @APP_PATCH_VERSION@ \n"
    "\#define APP_GIT_BRANCH \"@APP_GIT_BRANCH@\"\n"
    "\#define APP_GIT_COMMIT \"@APP_GIT_COMMIT@\"\n"
    "\#define BUILD_MACHINE \"@USERNAME@@@HOSTNAME@\"\n"
    "\#define BUILD_TYPE @BUILD_TYPE@ // 0-release, 1-debug\n"
    )

    configure_file(${PROJECT_BINARY_DIR}/asdk_version.h.in ${CMAKE_CURRENT_LIST_DIR}/common/asdk_version.h  @ONLY)
    file(WRITE ${CMAKE_CURRENT_LIST_DIR}/.version "${ASDK_MAJOR_VERSION}.${ASDK_MINOR_VERSION}.${ASDK_PATCH_VERSION}")

endfunction()