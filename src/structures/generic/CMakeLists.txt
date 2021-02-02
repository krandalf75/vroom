set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/edge.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/edge.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/matrix.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/matrix.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/undirected_graph.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/undirected_graph.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
