cmake_minimum_required(VERSION 3.13)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(own_project C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

# Add subdirectories:
add_subdirectory(test)
add_subdirectory(blink)
add_subdirectory(hello_gpio_irq)
add_subdirectory(hello_serial)
add_subdirectory(hello_pwm)
add_subdirectory(hello_adc)
add_subdirectory(hello_reset)
add_subdirectory(hello_timer)
add_subdirectory(hello_uart)
add_subdirectory(hello_watchdog)
add_subdirectory(lowlevel)
