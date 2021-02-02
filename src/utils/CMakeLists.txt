set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/exception.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/exception.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/input_parser.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/input_parser.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/output_json.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/output_json.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/version.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/helpers.h"
)

if(MSVC)
    list(APPEND private_src "${CMAKE_CURRENT_SOURCE_DIR}/getopt_win.h")
endif()

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
