set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/local_search.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/local_search.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/operator.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/operator.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
