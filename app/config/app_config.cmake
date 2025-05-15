# ----- User confiugration starts ----- #

### configure project name

SET(APP_NAME "ASDK user application")

### assign name to generate the elf, srec and hex file

SET(APP_ELF_NAME "asdk_app")

### file containing version information

SET(APP_VERSION_FILEPATH ${CMAKE_CURRENT_SOURCE_DIR}/version.txt)

### configure LD file

IF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M0PLUS")
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/app/linker_files/cyt2b75_cm0plus.ld
    )
ELSE()
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/app/linker_files/cyt2b75_cm4.ld
    )
ENDIF()

### add source files and include paths

SET(APP_USER_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    # add other include paths from here
)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app APP_USER_SRC)
# add other source paths from here

# ----- User confiugration ends ----- #
