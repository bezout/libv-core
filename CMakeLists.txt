cmake_minimum_required(VERSION 2.6)
project(libv-core)
set(CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR} ${PROJECT_SOURCE_DIR}/public/share/libv/core)

find_package(Boost COMPONENTS iostreams system thread)

set(libv_core_tests_libraries
${Boost_SYSTEM_LIBRARIES}
${Boost_THREAD_LIBRARIES}
)

include(libv_configure_library)

target_link_libraries(${PROJECT_NAME}
${Boost_IOSTREAMS_LIBRARIES}
)

configure_file(${PROJECT_SOURCE_DIR}/public/share/libv/libvConfig.cmake ${CMAKE_BINARY_DIR}/Findlibv.cmake COPYONLY)
