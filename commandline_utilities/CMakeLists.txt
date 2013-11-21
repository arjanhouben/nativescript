project( commandline_utilities )

include_directories( include )

add_library( commandline_utilities
    include/filesystem.h
    src/filesystem.cpp
    include/path.h
    src/path.cpp
    include/generic.h
#    src/generic.cpp
)
