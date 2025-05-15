# Application Software Development Kit (ASDK)

Application Software Development Kit (ASDK) is an embedded development platform which helps in writing application that is microcontroller agnostic.

### Supported Platforms

- Inifineon
    - [x] [Traveo 2 CYT2B75 starter kit](https://www.infineon.com/cms/en/product/evaluation-boards/cytvii-b-e-1m-sk/)

- STMicroelectronics
    - [ ] [STM32F4 Discovery](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)

- Texas Instruments
    - [ ] C2000

- NXP Semiconductors
    - [ ] [S32K144](https://www.nxp.com/design/design-center/development-boards-and-designs/automotive-development-platforms/s32k-mcu-platforms/s32k144-q100-evaluation-board-for-automotive-general-purpose:S32K144EVB)

- Renesas
    - [ ] RA6M4

## Folder structure

```sh
./
|__app/             # Contains user application
|__asdk_core/       # Contains asdk core API header files, docs...etc
|__catalyst/        # Contains middleware and libaries
|__examples/        # Contains peripheral examples regardless of platform choice
|__platform/        # Contains all of the supported microcontrollers (SDK + DAL)
|__build.py         # Build script
|__CMakeLists.txt   # The Top-level CMakeLists file
```

## Getting started

### 1. System requirements

Before going ahead please ensure that your system meets the following
requirements.

- One of the following host operating systems is installed.
  - [x] Windows 11 - (10.0.22621 Build 22621)
  - [x] Ubuntu - (20.04.5 LTS x86_64)
  - [x] macOS Ventura - (13.6.1 Apple M1)

- Python 3
    - [x] >= 3.8.x

### 2. Setup build environment

The `setup.py` script detects your operating system to download the right
set of required tools and the ARM GNU GCC toolchain. 

```sh
python ./asdk_core/setup/setup.py -p arm
```

<details close>
<summary>Click to see output:</summary>

```sh
INFO: getting cmake...
INFO: downloaded successfully
INFO: extracting 'cmake-3.26.4-windows-x86_64' to 'C:\Users\ajmeri.j\asdk_toolchain\cmake'...
INFO: extracted successfully
INFO: getting ninja...
INFO: downloaded successfully
INFO: extracting 'ninja.exe' to 'C:\Users\ajmeri.j\asdk_toolchain\ninja'...
INFO: extracted successfully
INFO: getting openocd...
INFO: downloaded successfully
INFO: extracting 'xpack-openocd-0.12.0-1' to 'C:\Users\ajmeri.j\asdk_toolchain\openocd'...
INFO: extracted successfully
INFO: getting 'arm' toolchain (version: 'gcc-arm-none-eabi-7-2018-q2-update')...
INFO: downloaded successfully
INFO: extracting '' to 'C:\Users\ajmeri.j\asdk_toolchain\arm\gcc-arm-none-eabi-7-2018-q2-update'...
INFO: extracted successfully
INFO: Setting envrionment variables...
INFO: Environment variables were added successfully.
```
</details>
<br>

> [!Important]
> After installing all the necessary tools. The script sets a few environment variables permanently to set the ASDK build environment. However, to bring them in effect in your current terminal session follow below instructions based on your operating system.

<details open>
<summary>Windows</summary>

    Close the command prompt and launch a new one. (OR)
    If you are using VS Code, close all instances of VS Code and re-open it.
</details>

<details open>
<summary>Ubuntu</summary>

```sh
source /Users/ajmeri.j/.bashrc
```
</details>

<details open>
<summary>macOS</summary>

```sh
source /Users/ajmeri.j/.bash_profile
```
</details>

### 3. Setup platform

### 4. Build application

To build the application run the build script as shown below.

```sh
python build.py -p cyt2b75_m0plus -b build -t debug
```

<details close>
<summary>Click to see output:</summary>

```sh
-- The C compiler identification is GNU 7.3.1
-- The CXX compiler identification is GNU 7.3.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Users/ajmeri.j/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Users/ajmeri.j/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-g++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
Application Version: 1.0.0
CMake version:3.26.4
Build type: Debug
CYT2B75_CORE: m0plus
-- The ASM compiler identification is GNU
-- Found assembler: C:/Users/ajmeri.j/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc.exe

Platform: CYT2B75

CYT2B75_CORE: m0plus
In Catalyst
-- Checking ASDK scheduler option
-- Checking ASDK scheduler option - disabled
-- Checking ASDK RTOS option
-- Checking ASDK RTOS option - disabled
-- Checking ASDK CAN Service option
-- Checking ASDK CAN Service option - disabled
-- Checking ASDK UDS option
-- Checking ASDK UDS option - disabled
-- Configuring done (4.5s)
-- Generating done (0.1s)
CMake Warning:
  Manually-specified variables were not used by the project:

    TARGET_RTOS


-- Build files have been written to: C:/Users/ajmeri.j/Desktop/work/github/asdk/build/debug
[65/67] Linking C executable asdk_app.elf
Memory region         Used Size  Region Size  %age Used
            SRAM:        3456 B        62 KB      5.44%
      CODE_FLASH:       16548 B       512 KB      3.16%
      WORK_FLASH:          0 GB        96 KB      0.00%
          SFLASH:          0 GB        32 KB      0.00%
      SFLASH_ALT:          0 GB        32 KB      0.00%
[67/67] cmd.exe /C "cd /D C:\Users\ajmeri.j\Desktop\work\github\asdk && C:\Users\ajmeri.j\asdk_toolchain\arm\gcc-arm-none-eabi-7-...ri.j/Desktop/work/github/asdk/build/debug/asdk_app.elf C:/Users/ajmeri.j/Desktop/work/github/asdk/build/debug/asdk_app_1.0.0.bin" 
build completed successfully!
```
</details>
<br>

> [!Important]
> Ensure that you are are able to build the project succesfully without any errors and warnings. If not then reach out to the ASDK team or raise an issue.

## How to run examples

All peripheral examples are listed under top-level `examples` directory. Each directory contains a readme file with brief explanation about the example and its build command.

However, the syntax is listed below for your reference.

```sh
python build.py -p cyt2b75_m0plus -e <example> -t debug
```

Where, &lt;example&gt; is one of the example source directory name.

To run the `./examples/gpio/blinky`, replace &lt;example&gt; with `blinky`.

```sh
python build.py -p cyt2b75_m0plus -e blinky -t debug
```

