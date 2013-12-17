project( nativescript )

cmake_minimum_required( VERSION 2.8 )

function( compare a b result )
	file( TO_CMAKE_PATH "${a}" a_n )
	string( TOLOWER "${a_n}" a_l )
	file( TO_CMAKE_PATH "${b}" b_n )
	string( TOLOWER "${b_n}" b_l )
	if( "${a_l}" STREQUAL "${b_l}" )
		set( ${result} TRUE PARENT_SCOPE )
	else()
		set( ${result} FALSE PARENT_SCOPE )
	endif()
endfunction()

if( WIN32 )
	add_definitions( -DNOMINMAX )
	set( HOMEDIR $ENV{HOMEDRIVE}$ENV{HOMEPATH} )
	file( TO_CMAKE_PATH "${HOMEDIR}" HOMEDIR )
else()
	set( HOMEDIR $ENV{HOME} )
endif()

compare( "${CMAKE_INSTALL_PREFIX}" "${HOMEDIR}" equal )

if( NOT equal )
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
	set( BIN_PREFIX ${NATIVESCRIPT_BASE_DIR}/ )
endif()

file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/.nativescript )

install(
	DIRECTORY
		${CMAKE_BINARY_DIR}/.nativescript
	DESTINATION
		${HOMEDIR}
	DIRECTORY_PERMISSIONS
		OWNER_WRITE OWNER_READ OWNER_EXECUTE
		GROUP_WRITE GROUP_READ GROUP_EXECUTE
		WORLD_WRITE WORLD_READ WORLD_EXECUTE
)

if( 0 )
	string( REPLACE ":" ";" path "$ENV{PATH}" )
	list( REMOVE_DUPLICATES path )
	foreach( p ${path} )
	message( "dir: ${p}" )
	endforeach()
endif()

if( APPLE )
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -Wall\ -Wfatal-errors\ -std=c++11\ -stdlib=libc++ )
elseif( MSVC )
	string( REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ /W4 )
else()
	set( CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}\ -Wall\ -Wfatal-errors\ -std=c++11 )
endif()

include_directories(
	include
	commandline_utilities/include
	${CMAKE_CURRENT_BINARY_DIR}
)

if( MSVC )
	set( DETECTED_INCLUDE_DIR "/I" )
	set( DETECTED_LIBRARY_DIR "/LIBPATH:" )
	set( DETECTED_LIBRARY_POSTFIX ".lib" )
	set( DETECTED_LINK_PREFIX "/link /DLL " )
	set( DETECTED_OUTPUT_CMD "/Fe" )
	set( DETECTED_OUTPUT_CMD "/OUT:" )
else()
	set( DETECTED_INCLUDE_DIR "-I" )
	set( DETECTED_LIBRARY_DIR "-L" )
	set( DETECTED_LIBRARY_CMD "-l " )
	set( DETECTED_OUTPUT_CMD "-o " )
endif()

configure_file( include/defines.in ${CMAKE_CURRENT_BINARY_DIR}/defines.h )

add_subdirectory( commandline_utilities )

add_executable( nativescript
	src/nativescript.cpp
	src/config.cpp
	include/config.h
	include/defines.in
)

target_link_libraries( nativescript
	commandline_utilities
)

install(
	TARGETS
		nativescript
	DESTINATION
		${BIN_PREFIX}bin
)
