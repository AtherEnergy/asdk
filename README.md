# Application Software Development Kit (ASDK)

ASDK is an embedded development platform designed to simplify application development across multiple microcontroller architectures. It provides a unified API and build system, enabling developers to write portable, microcontroller-agnostic applications.

## Supported Platforms

The table below summarizes the current status of platform and board support in ASDK. This helps users and contributors quickly identify which microcontrollers are ready, in progress, or planned for future support.

| Platform            | Board/MCU                                                                                                   | Status         | Development Branch |
|---------------------|------------------------------------------------------------------------------------------------------------|---------------|--------------------|
| Infineon            | [Traveo 2 CYT2B75 Starter Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cytvii-b-e-1m-sk/) | ✅ Supported       | main, development |
| STMicroelectronics  | [STM32F4 Discovery](https://www.st.com/en/evaluation-tools/stm32f4discovery.html)                          | 🟡 In Progress | platform/stm32 |
| Texas Instruments   | [F280039C](https://www.ti.com/tool/LAUNCHXL-F280039C), [F28P55X](https://www.ti.com/tool/LAUNCHXL-F28P55X)  | 🟡 In Progress | platform/c2000 |
| NXP                 | [S32K144](https://www.nxp.com/design/design-center/development-boards-and-designs/automotive-development-platforms/s32k-mcu-platforms/s32k144-q100-evaluation-board-for-automotive-general-purpose:S32K144EVB) | ⬜ Not Started | |
| Renesas             | [RA6M4](https://www.renesas.com/en/products/microcontrollers-microprocessors/ra-cortex-m-mcus/ek-ra6m4-evaluation-kit-ra6m4-mcu-group) | ⬜ Not Started | |

## Project Structure

The following layout provides an overview of the repository's organization. Each directory and file is structured to help contributors quickly locate application code, SDK components, platform support, build scripts, and documentation.

```
.
├── app/             # User application code and configuration
│   ├── inc/         # Application-specific headers
│   ├── config/      # Application configuration files
│   └── linker_files/# Linker scripts for supported MCUs
├── asdk_core/       # Core SDK: APIs, docs, setup scripts, utilities
│   ├── inc/         # Core API headers
│   ├── docs/        # Documentation and guides
│   ├── setup/       # Environment setup scripts and configs
│   └── utils/       # Helper scripts and templates
|   |__ .version     # ASDK version information
├── catalyst/        # Middleware, libraries, and services
├── examples/        # Peripheral and application examples
├── platform/        # Platform SDKs, device abstraction layers (DAL)
├── build.py         # Main build script
├── CMakeLists.txt   # Top-level CMake configuration
├── LICENSE          # Project license
├── README.md        # Project overview and instructions
└── version.txt      # Application version information
```

## Getting Started

Follow these steps to set up your development environment and build your first application with ASDK.

### Prerequisites

The following versions have been tested and are recommended for best results:

- **Operating System:**
  - Windows 11 (10.0.22621)
  - Ubuntu 20.04.5 LTS (x86_64)
  - macOS Ventura (13.6.1, Apple M1)
- **Python:** 3.8 or higher

> [!info]
> Please use these versions to ensure compatibility and a smooth development experience.

### 1. Clone the Repository

Clone ASDK to your local machine:

```
git clone https://github.com/AtherEnergy/asdk.git
cd asdk
```

### 2. Install Toolchain and Dependencies

For **ARM Cortex-M** series (Infineon, STMicroelectronics, NXP, Renesas):

Run the setup script to automatically download and configure the required tools and ARM GNU GCC toolchain:

```
python asdk_core/setup/setup.py -p arm
```

### 3. Update Environment Variables

After setup, environment variables are set permanently. To activate them in your current session:

- **Windows:**  
  Close and reopen your command prompt or VS Code.
- **Ubuntu:**  
  `source ~/.bashrc`
- **macOS:**  
  `source ~/.bash_profile`

### 4. Platform Setup

Follow the platform-specific setup instructions in [`platform/readme.md`](./platform/readme.md).

### 5. Build the Application

Build the default application for a supported platform:

```
python build.py -p cyt2b75_m0plus -b build -t debug -c
```

<details>
<summary>Example build output</summary>

```sh
-- The C compiler identification is GNU 7.3.1
-- The CXX compiler identification is GNU 7.3.1
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Check for working C compiler: C:/Users/<username>/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc.exe - skipped
-- Detecting C compile features
-- Detecting C compile features - done
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Check for working CXX compiler: C:/Users/<username>/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-g++.exe - skipped
-- Detecting CXX compile features
-- Detecting CXX compile features - done
Application Version: 1.0.0
CMake version:3.26.4
Build type: Debug
CYT2B75_CORE: m0plus
-- The ASM compiler identification is GNU
-- Found assembler: C:/Users/<username>/asdk_toolchain/arm/gcc-arm-none-eabi-7-2018-q2-update/bin/arm-none-eabi-gcc.exe

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
-- Configuring done (7.2s)
-- Generating done (0.1s)
CMake Warning:
  Manually-specified variables were not used by the project:

    TARGET_RTOS


-- Build files have been written to: C:/Users/<username>/Desktop/github/asdk/build/debug
[67/69] Linking C executable asdk_app.elf
Memory region         Used Size  Region Size  %age Used
            SRAM:        3456 B        62 KB      5.44%
      CODE_FLASH:       16496 B       512 KB      3.15%
      WORK_FLASH:          0 GB        96 KB      0.00%
          SFLASH:          0 GB        32 KB      0.00%
      SFLASH_ALT:          0 GB        32 KB      0.00%
[69/69] cmd.exe /C "cd /D C:\Users\<username>\Desktop\github\asdk && .../Users/<username>/Desktop/github/asdk/build/debug/asdk_app_1.0.0.bin 
build completed successfully!
```
</details>

> **Note:** Ensure the project builds without errors or warnings. If you encounter issues, please open an issue or contact the ASDK team.

### 6. Run Examples

Peripheral examples are located in the `examples/` directory. Each example includes a README with details and build instructions.

To build an example (e.g., `blinky`):

```
python build.py -p cyt2b75_m0plus -e blinky -t debug
```

---

## Contributing

Contributions are welcome! Please see [`asdk_core/docs/contributing.md`](./asdk_core/docs/contributing.md) for guidelines.

## License

This project is licensed under the terms of the [LICENSE](./LICENSE) file.

