target_sources(libvroom_example PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libvroom.cpp")

target_link_libraries(libvroom_example PRIVATE libvroom)
