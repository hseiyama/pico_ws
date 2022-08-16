add_executable(framework
        sys_main.c
        tgt_main.c
        )

# pull in common dependencies
target_link_libraries(framework pico_stdlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(framework)
