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

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(robot_car C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add wheel encoder library
add_library(wheel_encoder STATIC
        wheel_encoder.c
        )
target_link_libraries(wheel_encoder
        pico_stdlib              # for core functionality
        hardware_gpio            # for GPIO operations
        hardware_timer           # for timer functionality
        )

# Add main wheel encoder executable
add_executable(main_wheel_encoder
        main_wheel_encoder.c
        )
target_link_libraries(main_wheel_encoder
        pico_stdlib              # for core functionality
        wheel_encoder            # our wheel encoder library
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
        hardware_timer           # for timer functionality
        )

# Add main ultrasonic sensor executable
add_executable(main_ultrasonic_sensor
        main_ultrasonic_sensor.c
        )
target_link_libraries(main_ultrasonic_sensor
        pico_stdlib              # for core functionality
        ultrasonic_sensor        # our ultrasonic sensor library
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(main_ultrasonic_sensor)

pico_enable_stdio_usb(main_ultrasonic_sensor 1)
pico_enable_stdio_usb(main_wheel_encoder 1)
