project( commandline_utilities )

include_directories( include )

add_library( commandline_utilities
	include/commandline_utilities
	include/filesystem.h
	src/filesystem.cpp
	include/filesystem/path.h
	src/filesystem/path.cpp
	include/generic.h
	include/filesystem/directory.h
	src/filesystem/directory.cpp
#    src/generic.cpp
)

install(
	TARGETS
		commandline_utilities
	DESTINATION
		${NATIVESCRIPT_BASE_DIR}/lib
)

install(
	DIRECTORY
		include
	DESTINATION
		${NATIVESCRIPT_BASE_DIR}
)

if( INSTALL_SYMLINKS )

	file( WRITE ${CMAKE_CURRENT_BINARY_DIR}/make_symlinks.cmake
"
function( make_link src dst )
	message( STATUS \"creating symbolic link from \${src} to \${dst}\" )
	if( WIN32 )
		execute_process( COMMAND mklink \${dst} \${src} )
	else()
		execute_process( COMMAND \${CMAKE_COMMAND} -E create_symlink \${src} \${dst} )
	endif()
endfunction()
file( MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/ )
make_link( \"${CMAKE_INSTALL_PREFIX}/${NATIVESCRIPT_BASE_DIR}/lib\" \"${INSTALL_SYMLINKS}/lib\" )
make_link( \"${CMAKE_INSTALL_PREFIX}/${NATIVESCRIPT_BASE_DIR}/include\" \"${INSTALL_SYMLINKS}/include\" )
"
	)

	install( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/make_symlinks.cmake )

endif()
