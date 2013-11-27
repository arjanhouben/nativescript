project( nativescript )

cmake_minimum_required( VERSION 2.8 )

if( WIN32 )
	set( HOMEDIR $ENV{HOMEDRIVE}/$ENV{HOMEPATH} )
else()
	set( HOMEDIR $ENV{HOME} )
endif()

if( NOT ${CMAKE_INSTALL_PREFIX} STREQUAL ${HOMEDIR} )
	set( NATIVESCRIPT_BASE_DIR nativescript )
	message( STATUS "----------------------------------------" )
	message( STATUS "nativescript will install to ${CMAKE_INSTALL_PREFIX}/${NATIVESCRIPT_BASE_DIR}" )
	message( STATUS "installing might require root priviliges" )
	message( STATUS "if you want to install for current user only, please specify:" )
	if( NOT ${HOMEDIR} MATCHES "^[^ ]+$" )
		set( HOMEDIR "\"${HOMEDIR}\"" )
	endif()
	message( STATUS "cmake -DCMAKE_INSTALL_PREFIX=${HOMEDIR}" )
	message( STATUS "----------------------------------------" )
	set( INSTALL_SYMLINKS ${HOMEDIR}/.${NATIVESCRIPT_BASE_DIR} )
else()
	set( NATIVESCRIPT_BASE_DIR .nativescript )
endif()

file( MAKE_DIRECTORY .nativescript )

install(
	DIRECTORY
		.nativescript
	DESTINATION
		${HOMEDIR}
	DIRECTORY_PERMISSIONS
		OWNER_WRITE OWNER_READ OWNER_EXECUTE
		GROUP_WRITE GROUP_READ GROUP_EXECUTE
		WORLD_WRITE WORLD_READ WORLD_EXECUTE
)

set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -std=c++11\ -stdlib=libc++\ -Wall )

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

install(
	TARGETS
		nativescript
	DESTINATION
		bin
)
