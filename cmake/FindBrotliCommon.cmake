
#-------------------------------------------------------------------------------
# FindBrotliCommon.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(PkgConfig QUIET)
pkg_check_modules(PC_BrotliCommon QUIET brotlicommon)

find_path(BrotliCommon_INCLUDE_DIR
    NAMES types.h
    HINTS ${PC_BrotliCommon_INCLUDE_DIRS}
    PATH_SUFFIXES brotli)

find_library(BrotliCommon_LIBRARY
    NAMES brotlicommon
    HINTS ${PC_BrotliCommon_LIBRARY_DIRS})

set(BrotliCommon_VERSION ${PC_BrotliCommon_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BrotliCommon
    FOUND_VAR BrotliCommon_FOUND
    REQUIRED_VARS
        BrotliCommon_LIBRARY
        BrotliCommon_INCLUDE_DIR
        VERSION_VAR BrotliCommon_VERSION)

if(BrotliCommon_FOUND)
    set(BrotliCommon_LIBRARIES ${BrotliCommon_LIBRARY})
    set(BrotliCommon_INCLUDE_DIRS ${BrotliCommon_INCLUDE_DIR})
    set(BrotliCommon_DEFINITIONS ${PC_BrotliCommon_CFLAGS_OTHER})
endif()

if(BrotliCommon_FOUND AND NOT TARGET Brotli::BrotliCommon)
    add_library(Brotli::BrotliCommon UNKNOWN IMPORTED)
    set_target_properties(Brotli::BrotliCommon PROPERTIES
        IMPORTED_LOCATION "${BrotliCommon_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_BrotliCommon_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${BrotliCommon_INCLUDE_DIR}")
endif()

mark_as_advanced(
    BrotliCommon_INCLUDE_DIR
    BrotliCommon_LIBRARY)
