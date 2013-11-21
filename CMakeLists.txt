project( nativescript )

cmake_minimum_required( VERSION 2.8 )

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -std=c++11\ -stdlib=libc++\ -Weffc++ )

include_directories(
    include
    commandline_utilities/include
)

add_subdirectory( commandline_utilities )

add_executable( nativescript
    src/nativescript.cpp
    src/config.cpp
    include/config.h
)

target_link_libraries( nativescript
    commandline_utilities
)
