# Application configuration

```sh
./asdk-gen2-sample-app/app/config
├── app_config.cmake
```

The user should not modify the top level CMakeLists.txt file. Instead the user must use this file for following purposes.

**1. Project name** - This could be any string of your choice.

```cmake
SET(APP_NAME "ASDK user application")
```

**2. ELF filename** - This will be used to name the binary filename.

```cmake
SET(APP_ELF_NAME "asdk_app")
```

**3. Linker file** - Path to the linker file.

```cmake
SET(APP_LINKER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/app/linker_files/cm0plus.ld)
```

**4. Include filepath** - Include file paths.

```cmake
SET(APP_USER_INC 
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    # example:
    # ${CMAKE_CURRENT_SOURCE_DIR}/app/app_imu
)
```

**5. Source files** - Source file directories.

```cmake
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app APP_USER_SRC)
# example:
# AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app/app_imu APP_USER_SRC)
```
