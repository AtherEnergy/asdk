function(ASDK_COMPILE_APPLICATION)

    # Define the supported set of keywords
    set(prefix ARG)
    set(no_value_args)
    set(one_value_args
        APP_ELF
        APP_LINKER_FILE
    )
    set(multi_value_args
        APP_SRC
        APP_INC
        APP_USER_SRC
        APP_USER_INC
        USER_LINKER_OPTIONS
    )

    ### version info

    IF (EXISTS ${APP_VERSION_FILEPATH})
        file(READ ${APP_VERSION_FILEPATH} APP_VER)
        MESSAGE("Application Version: ${APP_VER}")
        string(REGEX REPLACE "\n$" "" APP_VER "${APP_VER}")
        string(REPLACE "." ";" APP_VERSION_LIST ${APP_VER})
        list(GET APP_VERSION_LIST 0 APP_MAJOR_VERSION)
        list(GET APP_VERSION_LIST 1 APP_MINOR_VERSION)
        list(GET APP_VERSION_LIST 2 APP_PATCH_VERSION)
        # SET(APP_ELF ${APP_ELF_NAME}_${APP_VER}.elf)
        SET(APP_ELF_NAME ${APP_ELF_NAME}_${APP_VER})
    ELSE()
        MESSAGE("Application Version: not specified")
        # SET(APP_ELF ${APP_ELF_NAME}.elf)
    endif()

    # parse function arguments

    cmake_parse_arguments(${prefix} "${no_value_args}" "${one_value_args}" "${multi_value_args}" ${ARGN})

    ### pre-checks

    # required: linker file

    # ASSERT_FILEPATH(${ARG_APP_LINKER_FILE} "application linker" true)

    # required: app sources and include paths

    # ASSERT_FILEPATH(${ARG_APP_SRC} "app source" true)
    # ASSERT_FILEPATH(${ARG_APP_INC} "app include" true)

    # asdk

    ADD_SUBDIRECTORY(${ASDK_DIR})

    ADD_EXECUTABLE(${ARG_APP_ELF}
        ${ARG_APP_SRC}
        ${ARG_APP_USER_SRC}
    )

    SET_TARGET_PROPERTIES(${ARG_APP_ELF} PROPERTIES LINKER_LANGUAGE C)

    ADD_DEPENDENCIES(${ARG_APP_ELF} asdk)

    # add include directories

    TARGET_INCLUDE_DIRECTORIES(
        ${ARG_APP_ELF}
        PRIVATE
            ${ARG_APP_INC}
            ${ARG_APP_USER_INC}
            ${ASDK_CORE_INC}
    )

    # linker flags

    TARGET_LINK_OPTIONS(
        ${ARG_APP_ELF}
        PRIVATE
            -Wl,-T${APP_LINKER_FILE}
            ${ARG_USER_LINKER_OPTIONS}
    )

    TARGET_LINK_LIBRARIES(
        ${ARG_APP_ELF}
        PRIVATE
            asdk
    )

    ### generate srec and hex files

    # srec generation

    add_custom_command(
        TARGET      ${ARG_APP_ELF}
        POST_BUILD
        COMMAND     ${CMAKE_COMPILER_PATH}objcopy
        ARGS        -O srec ${CMAKE_BINARY_DIR}/${ARG_APP_ELF} ${CMAKE_BINARY_DIR}/${APP_ELF_NAME}.srec
        BYPRODUCTS  ${CMAKE_BINARY_DIR}/${${ARG_APP_ELF}}.srec
    )

    # hex file generation

    add_custom_target(
        hex_output ALL
        COMMAND ${CMAKE_COMPILER_PATH}objcopy -O ihex $<TARGET_FILE:${ARG_APP_ELF}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_ELF_NAME}.hex
        DEPENDS ${ARG_APP_ELF}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    add_dependencies( hex_output ${ARG_APP_ELF})

    # bin file generation

    add_custom_target(bin_output ALL
        COMMAND ${CMAKE_COMPILER_PATH}objcopy -O binary $<TARGET_FILE:${ARG_APP_ELF}> ${CMAKE_CURRENT_BINARY_DIR}/${APP_ELF_NAME}.bin
        DEPENDS ${ARG_APP_ELF}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    )

    add_dependencies( bin_output hex_output)

endfunction()
