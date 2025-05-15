# Build system

The build system uses `CMake` with `Ninja` as its generator. Both are supported by major operating systems (`Windows`, `Ubuntu` and `Mac OS`) and is fairly easy to setup. ASDK provides a setup script `asdk-gen2/setup/setup.py` that downloads, installs and sets up the build environment in one step which gives the user with same experience regardless of the host operating system. It is the most quick and hassle-free way to setup the ASDK build environment on your local system.

> ASDK build environment is isolated from the host operating system.

Building a project with cmake generally requires more than a few command line parameters dependending of the project's complexity. These parameters are required to compile the source files. For instance, the target platform might define its own compiler flags to handle variants of different micrcontrollers in a family. Similary, an application user might add their own user defines. Then, ASDK developers might add a few more defines to make the build process modular. The list is highly extensible which makes the build command quite long. ASDK provides a build script `build.py` which is a wrapper around the cmake build command and makes the build command shorter and easy to remember. The build command is shown below.

```sh
python ./build.py --build build_m0 --platform cyt2b75_m0plus
```

Where, `--build` specifies the build output directory and `--platform` specifies the target microcontroller.

The build script is written in python for cross compatibility across operating systems and it does other helpful things.

* Right before triggering the build, it validates the following
    * Whether the build tools have been installed.
    * Whether the right toolchain is available for the specified target platform.
* If all looks good it triggers the build using cmake command.
* Otherwise, it suggests the corrective action required to proceed further with the build.

We will use `CYT2B75` as target platform for throught this guide as a reference.

## Overview

This section gives a brief idea about the entire build process. Let's consider a simple project like blinking an LED to understand the application build process. A typical application folder structure is shown below.

```sh
./asdk-gen2-sample-app
├── app                     # contains source code to blink an LED
├── asdk-gen2               # ASDK as a gitsubmodule
├── build.py                # the build script
├── CMakeLists.txt          # the top level cmake file
└── version.txt             # contains version info
```

The top-level `CMakeLists.txt` file is the starting point for the build process. It triggers the build recipe that compiles the application source files to produce an executable binary image. This cmake file specifies 3 necessary steps and is mentioned below.

#### 1.Include helper cmake files

The following helper cmake files are included at the beginning of the cmake file.

* `asdk_config.cmake` - ASDK user configuration file
    
    ```cmake
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/app/config/asdk_config.cmake)
    ```

    > Refer [ASDK configuration](./asdk_config.md) for more information.

* `app_config.cmake` - Application user configuration file

    ```cmake
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/app/config/app_config.cmake)
    ```

    > Refer [Application configuration](./app_config.md) for more information.

* `asdk_app_recipe.cmake` - Application build receipe
    
    ```cmake
    INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/asdk-gen2/cmake/asdk_app_recipe.cmake)
    ```

#### 2. List of source files with include paths.

* The `APP_INC` variable contains filepath to `app` directory that contains `*.h` files.
    ```cmake
    SET(APP_INC ${CMAKE_CURRENT_SOURCE_DIR}/app)
    ```
* The `APP_SRC` variable contains filepaths to all of the `*.c` files under `app` folder including the `main.c` file.
    ```cmake
    AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/app APP_SRC)
    ```

#### 3. Trigger build

Finally, a cmake function `ASDK_COMPILE_APPLICATION` is called to compile the application source files along with ASDK. This is where the build process begins.

```cmake
ASDK_COMPILE_APPLICATION(
    APP_ELF ${APP_ELF}

    APP_SRC ${APP_SRC}                    # main.c file
    APP_INC ${APP_INC}                    # directory of main.c
    APP_USER_SRC ${APP_USER_SRC}          # source files from user sub-directories if any
    APP_USER_INC ${APP_USER_INC}          # include path from user sub-directories if any
    APP_LINKER_FILE ${APP_LINKER_FILE}    # linker file

    USER_LINKER_OPTIONS ${USER_LINKER_OPTIONS}  # linker optionss
)
```

The next section describes the application build process.

## Application build process

The `ASDK_COMPILE_APPLICATION` function defines the application build process and is defined in the `asdk-gen2/cmake/asdk_app_recipe.cmake` file. The build process is split in two stages compiler stage and linker stage.

At the compiler stage application source files are compiled first, then ASDK is compiled as its dependency. Then, in the linker stage both the application and ASDK are linked to produce the binary file. Below is a snippet from the file for your reference.

**Compiler stage** - Compiles the application source files by adding ASDK as its dependency.

```cmake
ADD_SUBDIRECTORY(asdk-gen2)

ADD_EXECUTABLE(${ARG_APP_ELF}
    ${ARG_APP_SRC}
    ${ARG_APP_USER_SRC}
)

ADD_DEPENDENCIES(${ARG_APP_ELF} asdk)
```

**Linker stage** - Once ASDK is compiled, both gets linked together to generate the binary file.

```cmake
TARGET_LINK_LIBRARIES(
    ${ARG_APP_ELF}
    PRIVATE
        asdk
)
```

The next section describes the ASDK build process. ASDK developers must be familiar with it. However application developers, the users of ASDK can skip the next section.

## ASDK build process

ASDK uses layered architecture with each layer containig its own `CMakeLists.txt` file that describes its build recipe. It starts from the bottom most layer. Each layer is compiled to a static library. All static libraries of all layers together represent ASDK which then gets linked with the application to produce the final binary file.

The table below, starting from the bottom most layer, describes each layer and its corresponding output.

|Layer|Description|Output|
|-----|-----------|------------|
|**SDK**  |SDK of the target platform|`lib<platform>_sdk.a`|
|**DAL**  |Driver Abstraction Layer of the target platform|`lib<platform>_dal.a`|
|**Middleware**|Middleware modules if any|`libuds.a`, `libcan_service.a`...etc|
|**RTOS**|RTOS if any|`librtos.a`|

Where, &lt;platform&gt; value is the specified target.

### Platform

All the layers of ASDK with `*.c` files need the compiler flags which are defined by the SDK of target platform including linker flags. Sometimes assembler flags might be required. Exporting these flags to other layers is necessary to ensure the binary file is produced by using the same set of flags consistently across the layers.

Platform is a logical layer in the build recipe that represents both SDK and DAL layer combined. It exposes all the flags as cmake interface library.

The platform build recipe file location is shown below.
    
    asdk-gen2/platform/<target_platform>/CMakeLists.txt

The `CYT2B75` platform build recipe file location is shown below.

    asdk-gen2/platform/cyt2b75/CMakeLists.txt

Below is a snippet from the file that creates platform as interface library.

```cmake
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
```

#### SDK

The SDK layer is the bottom most layer and is the important layer as it contains compiler flags that are required for the target platform.  It has direct impact on debug and release builds. In general, it contains following files.

|Purpose|File Types|Description|
|----|----------|-----------|
|Startup files|*.s, *.c|Contains boot-up code and set's the C run time environment before calling `main()` function.|
|System files|*.c|Contains bare minimum clock setup required to run the microcontroller.|
|Drivers files|*.c|Contains peripheral drivers which are required by the system and the ASDK DAL layer.|

All other files within SDK are ignored.

The compiler flags are defined in `FindFlags.cmake` file alongside the top level `CMakeLists.txt` file of the platform. Below is a snippet from the cmake file that includes the compiler flags.

```cmake
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/FindFlags.cmake)
```

The compiler flags are exposed as SDK interface library in cmake. Below is a snippet from platform recipe.

```cmake
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
```

The below snippet generates static library for `CYT2B75` SDK with `libcyt2b75_sdk.a` file as output.

```cmake
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
```

#### Driver Abstraction Layer (DAL)

The driver abstraction layer is right above the SDK layer. Therefore, the SDK layer is added as its dependency. This layer is compiled to a static library using the compiler flags of the SDK layer. 

The below snippet generates static library for `CYT2B75` DAL with `libcyt2b75_dal.a` file as output. Note that the `cyt2b75_sdk_interface` is used for sharing compiler flags.

```cmake
ADD_LIBRARY(cyt2b75_dal STATIC ${DAL_SRC})

ADD_DEPENDENCIES(cyt2b75_dal cyt2b75_sdk)

TARGET_INCLUDE_DIRECTORIES(
    cyt2b75_dal
    PRIVATE # hidden from application
        ${DAL_INC}
        ${SDK_INC}
)

TARGET_LINK_LIBRARIES(
    cyt2b75_dal
    PRIVATE # hidden from application
        cyt2b75_sdk_interface # use platform compile options
        cyt2b75_sdk
)
```

### Middleware

The middleware within ASDK sits above the DAL layer. It is composed of several independent modules. Each module has its own `CMakeLists.txt` file which produces its own static libraries as output. The static library of each module is combined into a logical library just like platform as cmake interface library.

The top level cmake file for middleware.

    asdk-gen2/middleware/CMakeLists.txt

The ASDK configuration file can be used to either enable or disable a particular module within middleware. By design middleware is entirely optional, the user can just turn off all the modules of middleware. This is possible because cmake allows conditional linking based on the configuration options made available within the configuration file.

The ASDK configuration file in application.

    asdk-gen2-sample-app/app/config/asdK_config.cmake

The below snippet shows the recipe that produces middleware as interface library.

```cmake
ADD_LIBRARY(
    middleware
    INTERFACE
)

TARGET_LINK_LIBRARIES(
    middleware
    INTERFACE
        $<$<BOOL:${USE_SCHEDULER}>:scheduler>
)
```

#### Modules

Compiling a module is straight forward, just compile the source files with platform as its dependency. Additionally a macro is defined by each module that allows conditional compilation.

Below is a snippet from CAN services module. It defines a macro named `ASDK_USE_CAN_SERVICE` for conditional compilation.

```cmake
ADD_LIBRARY(can_service STATIC ${CAN_SERVICE_SRC})

ADD_DEPENDENCIES(can_service platform)

TARGET_INCLUDE_DIRECTORIES(
    can_service
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_COMPILE_DEFINITIONS(
    can_service
    PUBLIC
        -DASDK_USE_CAN_SERVICE=${ASDK_USE_CAN_SERVICE}
)

TARGET_LINK_LIBRARIES(
    can_service
    PRIVATE
        platform
)
```

Below is an option to include or exclude the module from application using the the ASDK configuration file.

    option(USE_CAN_SERVICE "Enable CAN Service" ON)

Similarly, all other modules in middleware provdide an option.

### RTOS

