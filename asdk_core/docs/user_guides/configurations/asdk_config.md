# ASDK configuration

```sh
./asdk-gen2-sample-app/app/config
└── asdk_config.cmake
```

Contains options to either enable or disable optional modules within ASDK. The middleware is entirely optional so if the user requires any module from middleware the user has to explicitly enable it.

For example, to enable/disable middleware modules like CAN Services, UDS or Scheduler the user must set the appropriate option as desired. The sample application doesn't use any of those so all options are turned off as shown below.

```cmake
option(USE_SCHEDULER "Enable ASDK Scheduler" OFF)
option(USE_RTOS "Enable RTOS" OFF)
option(USE_CAN_SERVICE "Enable CAN Service" OFF)
option(USE_UDS "Enable UDS" OFF)
```