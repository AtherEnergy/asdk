# Setup script

The setup script downloads necessary tools required for the build system, toolchains for compiling your project and sets the environment variables for compiling the source files. It takes two arguments from the command line, the target platform architecture and the toolchain version.

The supported platform and toolchain versions can be listed by simply running the help command. 

```sh
python setup.py --help
```

Output:
```sh
usage: setup.py [-h] [-p] [--arm-version  | --c2000-version ] [-f]

optional arguments:
  -h, --help        show this help message and exit
  -p , --platform   Install platform-specific toolchain. Allowed values: arm,
                    c2000 (default: arm)
  --arm-version     ARM toolchain version. Allowed values: [gcc-arm-none-
                    eabi-7-2018-q2-update] (default: gcc-arm-none-
                    eabi-7-2018-q2-update)
  --c2000-version   TI C2000 toolchain version. Allowed values: [None]
                    (default: None)
  -f, --force       Force install if the specified tools already installed.
                    (default: False)
```

> The default platform is `arm` with `gcc-arm-none-eabi-7-2018-q2-update` toolchain version if the user doesn't specify any arguments.

> The --force option is not implemented yet.

# Overview

The script creates a folder named `asdk_toolchain` in the user's home directory where it installs all the required tools. Folder structure is shown below for your reference.

```sh
/home/jaimin/asdk_toolchain
├── arm
│   └── gcc-arm-none-eabi-7-2018-q2-update
├── cmake-3.26.4-linux-x86_64
└── ninja
```

## Environment variables

The build script accesses following environment variables to build the application with these tools and it does not depend on the native tools which are available in the operating system. There are two types of variables.

* Build system variables
* Toolchain variables

### Build system variables

These variables are used for accessing the tools related to build system.

#### `ASDK_TOOLCHAIN_ROOT`

For accessing the `asdk_toolchain` directory. The below table lists the path to asdk_toolchain folder based on the operating system.

|OS|Home|
|--|----|
|Windows|`C:\Users\<username>\asdk_toolchain`|
|Ubuntu & MAC|`/home/<username>/asdk_toolchain`|

#### `ASDK_CMAKE_ROOT`

The build script accesses CMake using this variable.

```sh
<ASDK_TOOLCHAIN_ROOT>/cmake-3.26.4-linux-x86_64
```

#### `ASDK_NINJA_ROOT`

ASDK-GEN2 uses `Ninja` as its generator for CMake. The build script accesses Ninja using this variable.

```sh
<ASDK_TOOLCHAIN_ROOT>/ninja
```

### Toolchain variables

Toolchain variables are used by the cmake files for accessing the toolchain based on the target platform. The folder structure is organized in a way that allows multiple versions of toolchain to reside together. Which let's the user choose the desired version for building the application. The folder structure hierarchy is show below for your reference.

```sh
/home/jaimin/asdk_toolchain
├── <architecture>              
│   └── <toolchain-version-1>
│   └── <toolchain-version-2>
```

#### `ASDK_<ARCH>_TOOLCHAIN`

Points to the target architecture, where arch is replaced with architecture name.

#### `ASDK_<ARCH>_TOOLCHAIN_VERSION`

Points to the targeted toolchain version, where arch is replaced with architecture name.

#### An example of ARM architecture with GNU GCC toolchain.

Folder structure with ARM toolchain.

```sh
/home/jaimin/asdk_toolchain
├── arm
│   └── gcc-arm-none-eabi-7-2018-q2-update
```

`ASDK_ARM_TOOLCHAIN_ROOT`

```sh
<ASDK_TOOLCHAIN_ROOT>/arm
```

`ASDK_ARM_TOOLCHAIN_VERSION`

```sh
gcc-arm-none-eabi-7-2018-q2-update
```

The toolchain cmake file location which constructs the path to the toolchain is shown below for your reference.

```sh
asdk-gen2/cmake
├── devel_arm_eabi_toolchain.cmake
```

Below is a snippet from the above file.

```cmake
SET(ASDK_ARM_TOOLCHAIN_BIN $ENV{ASDK_ARM_TOOLCHAIN_ROOT}/$ENV{ASDK_ARM_TOOLCHAIN_VERSION}/bin)
```

## Configuration file

The setup script uses `setup.yaml` configuration file to maintain all the supported build system tools and platform toolchains. The script must be updated to handle any new additions.

Below is a snippet from the file which has links to `CMake` and `Ninja` for the supported operatign systems:

```yaml
requirements:
    cmake:
        windows:  https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-windows-x86_64.zip
        linux:  https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-linux-x86_64.tar.gz
        darwin: https://github.com/Kitware/CMake/releases/download/v3.26.4/cmake-3.26.4-macos10.10-universal.tar.gz
    ninja:
        windows:  https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-win.zip
        linux:  https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-linux.zip
        darwin: https://github.com/ninja-build/ninja/releases/download/v1.11.1/ninja-mac.zip
```

Below is a snippet from the file which has links to platform toolchain along with version number:

```yaml
toolchains:
    arm:
      # default version for arm
      gcc-arm-none-eabi-7-2018-q2-update:
        windows:  https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-win32.zip
        linux:  https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2
        darwin: https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-mac.tar.bz2
```

### How to add new toolchain

The syntax for toolchain:

```yaml
toolchains:
    <architecture>:
        <toolchain_version>:
            windows: <url to toolchain>
            linux: <url to toolchain>
            darwin: <url to toolchain>
```

Example:

```yaml
toolchains:
    arm:
        toolchain_v1:
            windows: <url to toolchain v1>
            linux: <url to toolchain v1>
            darwin: <url to toolchain v1>
        toolchain_v2:
            windows: <url to toolchain v2>
            linux: <url to toolchain v2>
            darwin: <url to toolchain v2>
```
