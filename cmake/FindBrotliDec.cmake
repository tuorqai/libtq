
#-------------------------------------------------------------------------------
# FindBrotliDec.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(BrotliCommon REQUIRED)

find_package(PkgConfig)
pkg_check_modules(PC_BrotliDec QUIET brotlidec)

find_path(BrotliDec_INCLUDE_DIR
    NAMES decode.h
    HINTS ${PC_BrotliDec_INCLUDE_DIRS}
    PATH_SUFFIXES brotli)

find_library(BrotliDec_LIBRARY
    NAMES brotlidec
    HINTS ${PC_BrotliDec_LIBRARY_DIRS})

set(BrotliDec_VERSION ${PC_BrotliDec_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(BrotliDec
    FOUND_VAR BrotliDec_FOUND
    REQUIRED_VARS
        BrotliDec_LIBRARY
        BrotliDec_INCLUDE_DIR
        VERSION_VAR BrotliDec_VERSION)

if(BrotliDec_FOUND)
    set(BrotliDec_LIBRARIES ${BrotliDec_LIBRARY})
    set(BrotliDec_INCLUDE_DIRS ${BrotliDec_INCLUDE_DIR})
    set(BrotliDec_DEFINITIONS ${PC_BrotliDec_CFLAGS_OTHER})
endif()

if(BrotliDec_FOUND AND NOT TARGET Brotli::BrotliDec)
    add_library(Brotli::BrotliDec UNKNOWN IMPORTED)
    set_target_properties(Brotli::BrotliDec PROPERTIES
        IMPORTED_LOCATION "${BrotliDec_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_BrotliDec_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${BrotliDec_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES Brotli::BrotliCommon)
endif()

mark_as_advanced(
    BrotliDec_INCLUDE_DIR
    BrotliDec_LIBRARY)
