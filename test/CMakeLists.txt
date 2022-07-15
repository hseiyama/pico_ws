add_executable(test
    test.c
)

# This will enable serial output via USB
#pico_enable_stdio_usb(test 1)
# This will enable serial output via UART
pico_enable_stdio_uart(test 1)

pico_add_extra_outputs(test)

target_link_libraries(test pico_stdlib)