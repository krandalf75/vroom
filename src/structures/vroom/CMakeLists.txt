add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/input)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/solution)

set(private_src 
    "${CMAKE_CURRENT_SOURCE_DIR}/amount.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/break.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/break.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/job.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/job.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/location.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/location.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_route.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/raw_route.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/solution_state.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/solution_state.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/time_window.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/time_window.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/tw_route.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/tw_route.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/vehicle.h"
)

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
