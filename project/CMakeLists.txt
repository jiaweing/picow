# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
include(${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
# ====================================================================================
set(PICO_BOARD pico_w CACHE STRING "Board type")

# Set FreeRTOS kernel path
set(FREERTOS_KERNEL_PATH /Users/jiawei/Documents/FreeRTOS-KernelV11.1.0)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Initialize the SDK
project(robot_car C CXX ASM)

# Initialize the Pico SDK
pico_sdk_init()

# Add FreeRTOS as a library
add_library(freertos STATIC
    ${FREERTOS_KERNEL_PATH}/tasks.c
    ${FREERTOS_KERNEL_PATH}/queue.c
    ${FREERTOS_KERNEL_PATH}/list.c
    ${FREERTOS_KERNEL_PATH}/timers.c
    ${FREERTOS_KERNEL_PATH}/event_groups.c
    ${FREERTOS_KERNEL_PATH}/stream_buffer.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/port.c
    ${FREERTOS_KERNEL_PATH}/portable/MemMang/heap_4.c
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0/portasm.c
)

target_include_directories(freertos PUBLIC
    ${FREERTOS_KERNEL_PATH}/include
    ${FREERTOS_KERNEL_PATH}/portable/GCC/ARM_CM0
    ${CMAKE_SOURCE_DIR}
    ${PICO_SDK_PATH}/src/common/pico_base/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_base/include
    ${PICO_SDK_PATH}/src/rp2040/hardware_regs/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_timer/include
    ${PICO_SDK_PATH}/src/common/pico_time/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_irq/include
    ${PICO_SDK_PATH}/src/rp2_common/hardware_sync/include
    ${PICO_SDK_PATH}/src/rp2_common/cmsis/include
)

target_link_libraries(freertos PUBLIC
    pico_stdlib
    hardware_exception
    hardware_irq
    cmsis_core
)

# Add wheel encoder library
add_library(wheel_encoder STATIC
        wheel_encoder.c
        )
target_link_libraries(wheel_encoder
        pico_stdlib              # for core functionality
        hardware_gpio            # for GPIO operations
        hardware_timer          # for timer functionality
        freertos                # FreeRTOS library
        )

# Add main wheel encoder executable
add_executable(main_wheel_encoder
        main_wheel_encoder.c
        )
target_link_libraries(main_wheel_encoder
        pico_stdlib              # for core functionality
        wheel_encoder            # our wheel encoder library
        freertos                # FreeRTOS library
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(main_wheel_encoder)

# Add ultrasonic sensor library
add_library(ultrasonic_sensor STATIC
        ultrasonic_sensor.c
        )
target_link_libraries(ultrasonic_sensor
        pico_stdlib              # for core functionality
        hardware_gpio            # for GPIO operations
        hardware_timer          # for timer functionality
        freertos                # FreeRTOS library
        )

# Add main ultrasonic sensor executable
add_executable(main_ultrasonic_sensor
        main_ultrasonic_sensor.c
        )
target_link_libraries(main_ultrasonic_sensor
        pico_stdlib              # for core functionality
        ultrasonic_sensor        # our ultrasonic sensor library
        freertos                # FreeRTOS library
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(main_ultrasonic_sensor)

# Enable USB output
pico_enable_stdio_usb(main_ultrasonic_sensor 1)
pico_enable_stdio_usb(main_wheel_encoder 1)

# Add global compile definitions
target_compile_definitions(freertos PUBLIC
    PICO_MAX_SHARED_IRQ_HANDLERS=16
    PICO_USE_MALLOC_MUTEX=1
    LIB_PICO_PLATFORM=1
)
