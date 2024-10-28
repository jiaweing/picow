cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(${CMAKE_CURRENT_SOURCE_DIR}/../pico_sdk_import.cmake)

project(pid_controller C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(pid_controller
    pid_controller.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(pid_controller pico_stdlib)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(pid_controller)

# Enable USB output, disable UART output
pico_enable_stdio_usb(pid_controller 1)
pico_enable_stdio_uart(pid_controller 0)
