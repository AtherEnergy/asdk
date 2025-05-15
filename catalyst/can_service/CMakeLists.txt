MESSAGE("In CAN Service")

SET(CAN_SERVICE_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/linked_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/asdk_can_service.c
)

ADD_LIBRARY(can_service STATIC ${CAN_SERVICE_SRC})

ADD_DEPENDENCIES(can_service platform)

TARGET_INCLUDE_DIRECTORIES(
    can_service
    PUBLIC
        ${ASDK_CORE_INC}
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
