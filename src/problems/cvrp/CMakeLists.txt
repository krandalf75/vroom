add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/operators)

set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/cvrp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cvrp.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
