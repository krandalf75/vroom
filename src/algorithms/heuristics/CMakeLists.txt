set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/solomon.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/solomon.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
