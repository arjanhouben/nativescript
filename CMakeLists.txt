project( nativescript )

cmake_minimum_required( VERSION 2.8 )

include_directories( include )

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -std=c++11\ -stdlib=libc++\ -Weffc++ )

add_executable( nativescript
    src/nativescript.cpp
    src/utilities.cpp
    include/utilities.h
    src/config.cpp
    include/config.h
)
