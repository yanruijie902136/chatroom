# ./libs/utils/CMakeLists.txt

cmake_minimum_required(VERSION 3.25)

set(LIBRARY_HEADERS
    ./include/utils/fdio.h
    )

set(LIBRARY_SOURCE
    ./src/fdio.c
    )

project(libutils)

add_library(utils STATIC
    ${LIBRARY_HEADERS}
    ${LIBRARY_SOURCE}
    )

target_include_directories(utils PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/utils/>
    $<INSTALL_INTERFACE:include/utils/>
    )

target_include_directories(utils PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
    $<INSTALL_INTERFACE:include/>
    )
