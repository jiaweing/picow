add_executable(exercise exercise.c)

# pull in common dependencies
target_link_libraries(exercise 
    pico_stdlib
    hardware_pwm
    hardware_adc
    hardware_timer
)

# enable usb output, enable uart output
pico_enable_stdio_usb(exercise 1)
pico_enable_stdio_uart(exercise 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(exercise)

# add url via pico_set_program_url
#example_auto_set_url(exercise)