
#-------------------------------------------------------------------------------
# FindVorbisFile.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(Vorbis REQUIRED)

find_package(PkgConfig)
pkg_check_modules(PC_VorbisFile QUIET vorbisfile)

find_path(VorbisFile_INCLUDE_DIR
    NAMES vorbisfile.h
    HINTS ${PC_VorbisFile_INCLUDE_DIRS}
    PATH_SUFFIXES vorbis)

find_library(VorbisFile_LIBRARY
    NAMES vorbisfile
    HINTS ${PC_VorbisFile_LIBRARY_DIRS})

set(VorbisFile_VERSION ${PC_VorbisFile_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VorbisFile
    FOUND_VAR VorbisFile_FOUND
    REQUIRED_VARS
        VorbisFile_LIBRARY
        VorbisFile_INCLUDE_DIR
        VERSION_VAR VorbisFile_VERSION)

if(VorbisFile_FOUND)
    set(VorbisFile_LIBRARIES ${VorbisFile_LIBRARY})
    set(VorbisFile_INCLUDE_DIRS ${VorbisFile_INCLUDE_DIR})
    set(VorbisFile_DEFINITIONS ${PC_VorbisFile_CFLAGS_OTHER})
endif()

if(VorbisFile_FOUND AND NOT TARGET Vorbis::VorbisFile)
    add_library(Vorbis::VorbisFile UNKNOWN IMPORTED)
    set_target_properties(Vorbis::VorbisFile PROPERTIES
        IMPORTED_LOCATION "${VorbisFile_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_VorbisFile_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${VorbisFile_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES Vorbis::Vorbis)
endif()

mark_as_advanced(
    VorbisFile_INCLUDE_DIR
    VorbisFile_LIBRARY)
