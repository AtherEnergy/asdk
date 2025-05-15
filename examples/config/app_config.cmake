# ----- User confiugration starts ----- #

### configure project name

SET(APP_NAME "ASDK user application")

### assign name to generate the elf, srec and hex file

SET(APP_ELF_NAME ${APP_ELF_NAME})

### configure LD file

IF(${TARGET_PLATFORM} STREQUAL "CYT2B75_M0PLUS")
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/linker_files/cyt2b75_cm0plus.ld
    )
ELSE()
    SET(APP_LINKER_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/examples/linker_files/cyt2b75_cm4.ld
    )
ENDIF()

### add source files and include paths

# SET(APP_USER_INC 
#     ${CMAKE_CURRENT_SOURCE_DIR}/app
#     # add other include paths from here

#     # example:
#     # ${CMAKE_CURRENT_SOURCE_DIR}/app/app_imu
# )

# AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app APP_USER_SRC)
# add other source paths from here

# example:
AUX_SOURCE_DIRECTORY(${APP_SRC_PATH} APP_SRC)

# ----- User confiugration ends ----- #
