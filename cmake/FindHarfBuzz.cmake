
#-------------------------------------------------------------------------------
# Findharfbuzz.cmake
# This file is part of tq library.
#-------------------------------------------------------------------------------

find_package(PkgConfig QUIET)
pkg_check_modules(PC_HarfBuzz QUIET harfbuzz)

find_path(HarfBuzz_INCLUDE_DIR
    NAMES hb.h
    HINTS ${PC_HarfBuzz_INCLUDE_DIRS}
    PATH_SUFFIXES harfbuzz)

find_library(HarfBuzz_LIBRARY
    NAMES harfbuzz
    HINTS ${PC_HarfBuzz_LIBRARY_DIRS})

set(HarfBuzz_VERSION ${PC_HarfBuzz_VERSION})

if(HarfBuzz_INCLUDE_DIR AND NOT HarfBuzz_VERSION)
    if(EXISTS "${HarfBuzz_INCLUDE_DIR}/hb-version.h")
        file(READ "${HarfBuzz_INCLUDE_DIR}/hb-version.h" _HarfBuzz_version_content)

        string(REGEX MATCH "#define +HB_VERSION_STRING +\"([0-9]+\.[0-9]+\.[0-9]+)\"" _dummy "${_HarfBuzz_version_content}")
        set(HarfBuzz_VERSION "${CMAKE_MATCH_1}")
    endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HarfBuzz
    FOUND_VAR HarfBuzz_FOUND
    REQUIRED_VARS
        HarfBuzz_LIBRARY
        HarfBuzz_INCLUDE_DIR
        VERSION_VAR HarfBuzz_VERSION)

if(HarfBuzz_FOUND)
    set(HarfBuzz_LIBRARIES ${HarfBuzz_LIBRARY})
    set(HarfBuzz_INCLUDE_DIRS ${HarfBuzz_INCLUDE_DIR})
    set(HarfBuzz_DEFINITIONS ${PC_HarfBuzz_CFLAGS_OTHER})
endif()

if(HarfBuzz_FOUND AND NOT TARGET HarfBuzz::HarfBuzz)
    add_library(HarfBuzz::HarfBuzz UNKNOWN IMPORTED)
    set_target_properties(HarfBuzz::HarfBuzz PROPERTIES
        IMPORTED_LOCATION "${HarfBuzz_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${PC_HarfBuzz_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${HarfBuzz_INCLUDE_DIR}")
endif()

mark_as_advanced(
    HarfBuzz_INCLUDE_DIR
    HarfBuzz_LIBRARY)
