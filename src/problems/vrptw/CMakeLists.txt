add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/operators)

set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/vrptw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vrptw.h"
)
target_sources(vroom PRIVATE ${private_src})
