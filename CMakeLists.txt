
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)
project(vroom LANGUAGES CXX C)

include(FindPkgConfig)
include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ language version to use (default is 17)")
set(CMAKE_CXX_STANDARD_REQUIRED True)

option(USE_LIBOSRM "Enable libosrm support" OFF)
option(USE_LIBGLPK "Enable libglpk support" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(CMAKE_BUILD_TYPE MATCHES Debug)
  message(STATUS "Configuring in debug mode")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  message(STATUS "Configuring in release mode")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
  message(STATUS "Configuring in release mode with debug flags")
elseif(CMAKE_BUILD_TYPE MATCHES MinRelSize)
  message(STATUS "Configuring in release mode with minimized size")
else()
  message(STATUS "Unrecognized build type - will use cmake defaults")
endif()

# Dependencies 
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(asio REQUIRED)

# main targets, sources are populated in subdirs
add_executable(vroom "")
add_library(libvroom STATIC "")

add_subdirectory(${CMAKE_SOURCE_DIR}/src)

target_link_libraries(vroom PRIVATE asio asio::asio)
target_link_libraries(vroom PRIVATE OpenSSL::Crypto)
target_link_libraries(vroom PRIVATE OpenSSL::SSL)
target_link_libraries(libvroom PUBLIC asio asio::asio)
target_link_libraries(libvroom PUBLIC OpenSSL::Crypto)
target_link_libraries(libvroom PUBLIC OpenSSL::SSL)

if(WIN32) 
  # prevent std::numeric_limits::max and other issues
  target_compile_definitions(vroom
    PUBLIC
      $<$<BOOL:${MSVC}>:NOMINMAX>
  )
  target_compile_definitions(libvroom
    PUBLIC
      $<$<BOOL:${MSVC}>:NOMINMAX>
  )
endif()

target_include_directories(vroom PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(vroom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(libvroom PUBLIC "${CMAKE_SOURCE_DIR}/include")
target_include_directories(libvroom PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
