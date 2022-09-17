add_executable(i2c_e2prom
        i2c_e2prom.c
        )

# pull in common dependencies
target_link_libraries(i2c_e2prom
        pico_stdlib
        hardware_i2c
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(i2c_e2prom)
