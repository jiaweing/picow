# This can be dropped into an external project to help locate the FreeRTOS kernel
# It should be included prior to the project() call.

if (NOT FREERTOS_KERNEL_PATH)
    message(FATAL_ERROR "FREERTOS_KERNEL_PATH not specified")
endif()

add_library(FreeRTOS-Kernel INTERFACE)

target_sources(FreeRTOS-Kernel INTERFACE
    ${FREERTOS_KERNEL_PATH}/croutine.c
    ${FREERTOS_KERNEL_PATH}/event_groups.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/stream_buffer.c
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/port.c
)

target_include_directories(FreeRTOS-Kernel INTERFACE
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0
    ${CMAKE_SOURCE_DIR}
)

target_compile_definitions(FreeRTOS-Kernel INTERFACE
    PICO_CONFIG_RTOS_ADAPTER_HEADER="${CMAKE_CURRENT_LIST_DIR}/FreeRTOSConfig.h"
)

add_library(FreeRTOS-Kernel-Heap4 INTERFACE)
target_sources(FreeRTOS-Kernel-Heap4 INTERFACE
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
)
