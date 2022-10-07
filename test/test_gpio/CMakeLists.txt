add_executable(test_gpio
        test_gpio.c
        )

# pull in common dependencies
target_link_libraries(test_gpio
        pico_stdlib
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(test_gpio)
