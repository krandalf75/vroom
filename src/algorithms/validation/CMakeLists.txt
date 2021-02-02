set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/check.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/check.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/choose_eta.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/choose_eta.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
