add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/cvrp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tsp)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vrptw)

set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/vrp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vrp.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
