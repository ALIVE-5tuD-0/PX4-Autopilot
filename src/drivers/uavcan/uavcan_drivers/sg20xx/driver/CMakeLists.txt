include_directories(
    ./include
    )

add_compile_options(-Wno-unused-variable)
add_library(uavcan_sg20xx_driver STATIC
    src/uc_sg20xx_thread.cpp
    src/uc_sg20xx_clock.cpp
    src/uc_sg2002_can.cpp
    )

add_dependencies(uavcan_sg20xx_driver uavcan)

install(DIRECTORY include/uavcan_nuttx DESTINATION include)
install(TARGETS uavcan_sg20xx_driver DESTINATION lib)

# vim: set et ft=cmake fenc=utf-8 ff=unix sts=4 sw=4 ts=4 :)
