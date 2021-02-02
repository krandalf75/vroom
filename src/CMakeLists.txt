add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/algorithms)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/problems)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/routing)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/structures)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/utils)

target_sources(vroom PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
