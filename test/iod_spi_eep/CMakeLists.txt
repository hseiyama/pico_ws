add_executable(iod_spi_eep
        iod_spi_eep.c
        )

# pull in common dependencies
target_link_libraries(iod_spi_eep
        pico_stdlib
        hardware_spi
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(iod_spi_eep)
