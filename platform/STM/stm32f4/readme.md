# Setup STM32F4 Platform

This document provides instructions for setting up the STM32F4 platform for development with the ASDK. Please follow the steps below to ensure a smooth and successful setup of the STM32CubeF4 SDK.

ASDK leverages the open-source [STM32CubeF4](https://github.com/STMicroelectronics/STM32CubeF4) package, which is included as a Git submodule. This integration streamlines the setup process. To initialize the submodule, execute the following commands in your terminal:

```sh
cd asdk
git submodule update --init --recursive -- platform\STM\stm32f4\sdk
```

✅ You have successfully set up the SDK, you can continue to to build and develop your application using the STM32F4 platform by following the link below.

[← Back to Getting Started](../../../README.md/#4-platform-setup)

---
