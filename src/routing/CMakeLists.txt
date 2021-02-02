set(private_src
    "${CMAKE_CURRENT_SOURCE_DIR}/http_wrapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/http_wrapper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/ors_wrapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ors_wrapper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/osrm_routed_wrapper.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/osrm_routed_wrapper.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/wrapper.h"
)

if(USE_LIBOSRM)
    list(APPEND private_src
        "${CMAKE_CURRENT_SOURCE_DIR}/libosrm_wrapper.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/libosrm_wrapper.h"
    )
endif()

target_sources(vroom PRIVATE ${private_src})
target_sources(libvroom PRIVATE ${private_src})
