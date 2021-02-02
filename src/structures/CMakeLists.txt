add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/generic)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vroom)

set(private_args
    "${CMAKE_CURRENT_SOURCE_DIR}/cl_args.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cl_args.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/typedefs.h"
)

target_sources(vroom PRIVATE ${private_args})
target_sources(libvroom PRIVATE ${private_args})
