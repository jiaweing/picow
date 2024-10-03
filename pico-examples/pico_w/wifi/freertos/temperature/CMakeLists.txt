cmake_minimum_required(VERSION 3.13)

# Initialize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "/Users/jiawei/.pico-sdk/sdk/2.0.0")

set(PICO_BOARD pico_w)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

# Set the path to FreeRTOS kernel
set(FREERTOS_KERNEL_PATH "/Users/jiawei/Documents/FreeRTOS-KernelV11.1.0")

# Pull in FreeRTOS
include(${CMAKE_CURRENT_LIST_DIR}/../FreeRTOS_Kernel_import.cmake)

project(picow_freertos_temperature C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (EXISTS ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c)
    add_executable(picow_freertos_temperature_nosys
            picow_freertos_temperature.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(picow_freertos_temperature_nosys PRIVATE
            WIFI_SSID="COSMOS"
            WIFI_PASSWORD="jiaweiii"
            LWIP_RAW=1
            LWIP_ICMP=1
            )
    target_include_directories(picow_freertos_temperature_nosys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/.. # for our common FreeRTOSConfig
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
            ${PICO_SDK_PATH}/lib/lwip/src/include # Add lwIP core include directory
            )
    target_link_libraries(picow_freertos_temperature_nosys
            hardware_adc
            pico_cyw43_arch_lwip_threadsafe_background
            pico_stdlib
            pico_lwip_iperf
            pico_lwip # Explicitly link against lwIP
            FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
            )
    pico_add_extra_outputs(picow_freertos_temperature_nosys)

    add_executable(picow_freertos_temperature_sys
            picow_freertos_temperature.c
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping/ping.c
            )
    target_compile_definitions(picow_freertos_temperature_sys PRIVATE
            WIFI_SSID="COSMOS"
            WIFI_PASSWORD="jiaweiii"
            NO_SYS=0            # don't want NO_SYS (generally this would be in your lwipopts.h)
            LWIP_SOCKET=1       # we need the socket API (generally this would be in your lwipopts.h)
            PING_USE_SOCKETS=1
            LWIP_RAW=1
            LWIP_ICMP=1
            )
    target_include_directories(picow_freertos_temperature_sys PRIVATE
            ${CMAKE_CURRENT_LIST_DIR}
            ${CMAKE_CURRENT_LIST_DIR}/.. # for our common FreeRTOSConfig
            ${CMAKE_CURRENT_LIST_DIR}/../.. # for our common lwipopts
            ${PICO_LWIP_CONTRIB_PATH}/apps/ping
            ${PICO_SDK_PATH}/src/rp2_common/pico_lwip/include
            ${PICO_SDK_PATH}/lib/lwip/src/include # Add lwIP core include directory
            )
    target_link_libraries(picow_freertos_temperature_sys
            hardware_adc
            pico_cyw43_arch_lwip_sys_freertos
            pico_stdlib
            pico_lwip_iperf
            pico_lwip # Explicitly link against lwIP
            FreeRTOS-Kernel-Heap4 # FreeRTOS kernel and dynamic heap
            )
    pico_add_extra_outputs(picow_freertos_temperature_sys)
endif()

pico_enable_stdio_usb(picow_freertos_temperature_sys 1)
pico_enable_stdio_uart(picow_freertos_temperature_sys 0)