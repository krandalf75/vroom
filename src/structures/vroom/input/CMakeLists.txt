set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/input.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_step.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle_step.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
