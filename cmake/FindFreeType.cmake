
#-------------------------------------------------------------------------------
# FindFreeType.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(BrotliDec REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

find_package(PkgConfig QUIET)
pkg_check_modules(PC_FreeType QUIET harfbuzz)

find_path(FreeType_INCLUDE_DIR
    NAMES ft2build.h
    HINTS ${PC_FreeType_INCLUDE_DIRS}
    PATH_SUFFIXES freetype2)

find_library(FreeType_LIBRARY
    NAMES freetype
    HINTS ${PC_FreeType_LIBRARY_DIRS})

set(FreeType_VERSION ${PC_FreeType_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FreeType
    FOUND_VAR FreeType_FOUND
    REQUIRED_VARS
        FreeType_LIBRARY
        FreeType_INCLUDE_DIR
        VERSION_VAR FreeType_VERSION)

if(FreeType_FOUND)
    set(FreeType_LIBRARIES ${FreeType_LIBRARY})
    set(FreeType_INCLUDE_DIRS ${FreeType_INCLUDE_DIR})
    set(FreeType_DEFINITIONS ${PC_FreeType_CFLAGS_OTHER})
endif()

if(FreeType_FOUND AND NOT TARGET FreeType::FreeType)
    add_library(FreeType::FreeType UNKNOWN IMPORTED)
    set_target_properties(FreeType::FreeType PROPERTIES
        IMPORTED_LOCATION "${FreeType_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_FreeType_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${FreeType_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "Brotli::BrotliDec;PNG::PNG;ZLIB::ZLIB")
endif()

mark_as_advanced(
    FreeType_INCLUDE_DIR
    FreeType_LIBRARY)
