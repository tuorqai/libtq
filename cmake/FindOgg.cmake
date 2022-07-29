
#-------------------------------------------------------------------------------
# FindOgg.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(PkgConfig)
pkg_check_modules(PC_Ogg QUIET ogg)

find_path(Ogg_INCLUDE_DIR
    NAMES ogg.h
    HINTS ${Ogg_INCLUDE_DIR} ${PC_Ogg_INCLUDE_DIRS}
    PATH_SUFFIXES ogg)

find_library(Ogg_LIBRARY
    NAMES ogg
    HINTS ${Ogg_LIBRARY_DIR} ${PC_Ogg_LIBRARY_DIRS})

set(Ogg_VERSION ${PC_Ogg_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Ogg
    FOUND_VAR Ogg_FOUND
    REQUIRED_VARS
        Ogg_LIBRARY
        Ogg_INCLUDE_DIR
        VERSION_VAR Ogg_VERSION)

if(Ogg_FOUND)
    set(Ogg_LIBRARIES ${Ogg_LIBRARY})
    set(Ogg_INCLUDE_DIRS ${Ogg_INCLUDE_DIR})
    set(Ogg_DEFINITIONS ${PC_Ogg_CFLAGS_OTHER})
endif()

if(Ogg_FOUND AND NOT TARGET Ogg::Ogg)
    add_library(Ogg::Ogg UNKNOWN IMPORTED)
    set_target_properties(Ogg::Ogg PROPERTIES
        IMPORTED_LOCATION "${Ogg_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_Ogg_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${Ogg_INCLUDE_DIR}")
endif()

mark_as_advanced(
    Ogg_INCLUDE_DIR
    Ogg_LIBRARY)
