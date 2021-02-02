set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/computing_times.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/computing_times.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/route.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/route.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/solution.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/solution.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/step.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/step.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/summary.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/summary.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/violations.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/violations.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
