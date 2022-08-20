add_executable(framework2
        sys_main.c
        apl_main.c
        iod_main.c
        )

# pull in common dependencies
target_link_libraries(framework2 pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(framework2)
