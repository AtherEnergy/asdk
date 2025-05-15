# Getting started

This document will help you setup the build environment of ASDK. We will start by cloning a sample ASDK application that will blink an LED which is the "hello world!" equivalent in embedded systems. Then, we will setup the ASDK build environment to compile this application. Successful compilation indicates that your setup is verified.

Just follow along and simply copy-paste the code-snippets into your terminal.

We will use the [CYTVII-B-E-1M-SK](https://www.infineon.com/cms/en/product/evaluation-boards/cytvii-b-e-1m-sk/) which is the Traveo T2G body entry starter kit as our reference board throught this guide.

## System requirements

Before going ahead, ensure that your system requirements are met. You will need to install `Git` to clone sample application. If you don't have it please install it from [this](https://git-scm.com/downloads) link.

### Host OS

Following host operating systems are supported and verified.

1. Windows 11 - `10.0.22621 Build 22621` [tested]
2. Ubuntu - `20.04.5 LTS x86_64` [tested]
3. Mac OS [tested]

### Supported Python versions

Following python versions are supported and verified.

* Python >= `v2.7.x` [tested]
* Python >= `v3.8.x` [tested]

## Clone Sample Application

To clone the sample application just copy the below snippet and run it your CLI terminal.

```sh
git clone --recursive git@gitlab.atherengineering.in:AtherEnergy/vehicle/asdk/gen2/asdk-gen2-sample-app.git
```

## Setup build environment and toolchain

The setup script `asdk-gen2/setup/setup.py` downloads all the required toolchain from the internet and will setup the build environment of ASDK.

Run the setup script.

```sh
python ./asdk-gen2/setup/setup.py --platform arm --arm-version gcc-arm-none-eabi-7-2018-q2-update
```

*Output:*

```sh
INFO: getting cmake...
INFO: downloaded successfully
INFO: extracting 'cmake-3.26.4-linux-x86_64' to '/home/jaimin/asdk_toolchain'...
INFO: extracted successfully
INFO: getting ninja...
INFO: downloaded successfully
INFO: extracting 'cmake-3.26.4-linux-x86_64.tar.gz' to '/home/jaimin/asdk_toolchain/ninja'...
INFO: extracted successfully
INFO: getting 'arm' toolchain (version: 'gcc-arm-none-eabi-7-2018-q2-update')...
INFO: downloaded successfully
INFO: extracting 'gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2' to '/home/jaimin/asdk_toolchain/arm'...
INFO: extracted successfully
INFO: Setting envrionment variables...
INFO: setting 'ASDK_TOOLCHAIN_ROOT' variable
INFO: setting 'ASDK_ARM_TOOLCHAIN_VERSION' variable
INFO: setting 'ASDK_ARM_TOOLCHAIN_ROOT' variable
INFO: setting 'ASDK_NINJA_ROOT' variable
INFO: setting 'ASDK_CMAKE_ROOT' variable
INFO: ASDK environment setup completed successfully!
Run 'source /home/jaimin/.bashrc'
```

The build environment variables have been set. To proceed further follow below instructions based on your operating system. 

* On `Windows`
    ```
    open a new command prompt and then procced.
    ```

* On `Ubuntu` run
    ```sh
    source ~/.bashrc
    ```

* On `MAC` run
    ```sh
    source ~/.bash_profile
    ```

## Build application

After setting up the toolchain and build environment we can now build the application. A build script `build.py` triggers the build process. On successful build, it generates build artifacts which can then be downloaded into the target hardware.

* Change the current working directory.

    ```sh
    cd asdk-gen2-sample-application
    ```

* Run the build command.

    ```sh
    python build.py --build build_m0 --platform cyt2b75_m0plus
    ```

    *Output:*
    ```sh
    -- The C compiler identification is GNU 7.3.1
    -- The CXX compiler identification is GNU 7.3.1
    -- Detecting C compiler ABI info
    -- Detecting C compiler ABI info - done
    -- Check for working C compiler: /home/jaimin/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc - skipped
    -- Detecting C compile features
    -- Detecting C compile features - done
    -- Detecting CXX compiler ABI info
    -- Detecting CXX compiler ABI info - done
    -- Check for working CXX compiler: /home/jaimin/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-g++ - skipped
    -- Detecting CXX compile features
    -- Detecting CXX compile features - done
    CMake version:3.26.4

    Platform: CYT2B75

    -- The ASM compiler identification is GNU
    -- Found assembler: /home/jaimin/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc
    In Middleware
    -- Checking ASDK scheduler option
    -- Checking ASDK scheduler option - disabled
    -- Configuring done (0.2s)
    -- Generating done (0.0s)
    -- Build files have been written to: /home/jaimin/work/asdk2/asdk-gen2-sample-app/build
    [54/54] cd /home/jaimin/work/asdk2/asdk...asdk-gen2-sample-app/build/asdk_app.hex
    build completed succesfully!
    ```

Ensure that you are are able to build the project succesfully without any errors and warnings. If not then reach out to the ASDK team.
