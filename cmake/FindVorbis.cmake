
#-------------------------------------------------------------------------------
# FindVorbis.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(Ogg REQUIRED)

find_package(PkgConfig QUIET)
pkg_check_modules(PC_Vorbis QUIET vorbis)

find_path(Vorbis_INCLUDE_DIR
    NAMES codec.h
    HINTS ${PC_Vorbis_INCLUDE_DIRS}
    PATH_SUFFIXES vorbis)

find_library(Vorbis_LIBRARY
    NAMES vorbis
    HINTS ${PC_Vorbis_LIBRARY_DIRS})

set(Vorbis_VERSION ${PC_Vorbis_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbis
    FOUND_VAR Vorbis_FOUND
    REQUIRED_VARS
        Vorbis_LIBRARY
        Vorbis_INCLUDE_DIR
        VERSION_VAR Vorbis_VERSION)

if(Vorbis_FOUND)
    set(Vorbis_LIBRARIES ${Vorbis_LIBRARY})
    set(Vorbis_INCLUDE_DIRS ${Vorbis_INCLUDE_DIR})
    set(Vorbis_DEFINITIONS ${PC_Vorbis_CFLAGS_OTHER})
endif()

if(Vorbis_FOUND AND NOT TARGET Vorbis::Vorbis)
    add_library(Vorbis::Vorbis UNKNOWN IMPORTED)
    set_target_properties(Vorbis::Vorbis PROPERTIES
        IMPORTED_LOCATION "${Vorbis_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_Vorbis_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${Vorbis_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES Ogg::Ogg)
endif()

mark_as_advanced(
    Vorbis_INCLUDE_DIR
    Vorbis_LIBRARY)
