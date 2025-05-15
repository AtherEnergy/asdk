Message("In Catalyst")

MESSAGE(CHECK_START "Checking ASDK scheduler option")
IF (USE_SCHEDULER)
    MESSAGE(CHECK_PASS "enabled")
    ADD_DEFINITIONS(-DASDK_USE_SCHEDULER=1)
    ADD_SUBDIRECTORY(scheduler)
ELSE()
    MESSAGE(CHECK_FAIL "disabled")
ENDIF()

MESSAGE(CHECK_START "Checking ASDK RTOS option")
IF(USE_RTOS)
    MESSAGE(CHECK_PASS "enabled")
    ADD_DEFINITIONS(-DASDK_USE_RTOS=1)
    ADD_SUBDIRECTORY(rtos)
ELSE()
    MESSAGE(CHECK_FAIL "disabled")
ENDIF()

MESSAGE(CHECK_START "Checking ASDK CAN Service option")
IF(USE_CAN_SERVICE)
    MESSAGE(CHECK_PASS "enabled")
    SET(ASDK_USE_CAN_SERVICE 1)
    ADD_SUBDIRECTORY(can_service)
ELSE()
    SET(ASDK_USE_CAN_SERVICE 0)
    MESSAGE(CHECK_FAIL "disabled")
ENDIF()

MESSAGE(CHECK_START "Checking ASDK UDS option")
IF(USE_UDS)
    MESSAGE(CHECK_PASS "enabled")
    SET(ASDK_USE_UDS 1)
    ADD_SUBDIRECTORY(uds)
ELSE()
    SET(ASDK_USE_UDS 0)
    MESSAGE(CHECK_FAIL "disabled")
ENDIF()

ADD_LIBRARY(
    catalyst
    INTERFACE
)

TARGET_LINK_LIBRARIES(
    catalyst
    INTERFACE
        $<$<BOOL:${USE_SCHEDULER}>:scheduler>
        $<$<BOOL:${USE_RTOS}>:rtos>
        $<$<BOOL:${USE_CAN_SERVICE}>:can_service>
        $<$<BOOL:${USE_UDS}>:uds>
)
