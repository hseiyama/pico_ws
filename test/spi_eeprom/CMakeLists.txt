add_executable(spi_eeprom
        spi_eeprom.c
        )

# pull in common dependencies
target_link_libraries(spi_eeprom
        pico_stdlib
        hardware_spi
        )

# create map/bin/hex file etc.
pico_add_extra_outputs(spi_eeprom)
