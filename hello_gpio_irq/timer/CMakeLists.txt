add_executable(timer timer.c)

# pull in common dependencies
target_link_libraries(timer pico_stdlib)

# enable usb output, enable uart output
pico_enable_stdio_usb(timer 1)
pico_enable_stdio_uart(timer 1)

# create map/bin/hex file etc.
pico_add_extra_outputs(timer)

# add url via pico_set_program_url
#example_auto_set_url(timer)