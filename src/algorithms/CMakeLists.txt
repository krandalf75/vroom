add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/heuristics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/local_search)

if(USE_LIBGLPK)
    find_package(GLPK REQUIRED)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/validation)
endif()

set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/kruskal.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/kruskal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/munkres.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/munkres.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
