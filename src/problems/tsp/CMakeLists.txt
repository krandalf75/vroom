add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/heuristics)

set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/tsp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tsp.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
