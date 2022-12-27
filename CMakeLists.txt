
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

#-------------------------------------------------------------------------------

# Quite dirty regex to pick up version from tq.h
set(TQ_VERSION_REGEX "#define TQ_VERSION[ \t]+\"(.+)\"")

# Read a line with version #define to TQ_VERSION variable
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/include/tq/tq.h"
    TQ_VERSION
        REGEX ${TQ_VERSION_REGEX})

# Leave just version number in TQ_VERSION
string(REGEX REPLACE ${TQ_VERSION_REGEX} "\\1" TQ_VERSION "${TQ_VERSION}")

# Finally, declare project()
project(tq
    VERSION ${TQ_VERSION}
    DESCRIPTION "Easy-to-use C99 library for simple 2D games"
    LANGUAGES C)

#-------------------------------------------------------------------------------
# Options

option(BUILD_SHARED_LIBS "Build shared libraries" NO)
option(BUILD_EXAMPLES "Build examples" YES)
option(USE_GLES2 "Force OpenGL ES 2.0 usage on Linux" NO)
option(USE_HARFBUZZ "Enable HarfBuzz (recommended)" YES)

#-------------------------------------------------------------------------------
# Detect target platform

if(EMSCRIPTEN)
    set(TQ_PLATFORM "Emscripten")
elseif(ANDROID)
    set(TQ_PLATFORM "Android")
elseif(WIN32)
    set(TQ_PLATFORM "Win32")
elseif(UNIX)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(TQ_PLATFORM "Linux")
    endif()
endif()

if(NOT DEFINED TQ_PLATFORM)
    message(FATAL_ERROR "Sorry, but your platform is out of luck.")
endif()

#-------------------------------------------------------------------------------
# Common properties

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_VISIBILITY_PRESET hidden)

#-------------------------------------------------------------------------------
# Compiler settings

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(
        -Wall
        -Wextra
        -Wno-switch
        -Wno-switch-default
        -Wno-unused-parameter
        -Wno-unused-variable)
endif()

#-------------------------------------------------------------------------------
# tq target

add_library(tq
    "src/tq.c"
    "src/tq_al_audio.c"
    "src/tq_android_display.c"
    "src/tq_audio_dec.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_error.c"
    "src/tq_gl_renderer.c"
    "src/tq_gles2_renderer.c"
    "src/tq_graphics.c"
    "src/tq_image_loader.c"
    "src/tq_log.c"
    "src/tq_math.c"
    "src/tq_null_audio.c"
    "src/tq_null_renderer.c"
    "src/tq_posix_clock.c"
    "src/tq_posix_threads.c"
    "src/tq_sdl_display.c"
    "src/tq_stream.c"
    "src/tq_text.c"
    "src/tq_win32_clock.c"
    "src/tq_win32_display.c"
    "src/tq_win32_threads.c")

target_include_directories(tq PUBLIC "include")
target_include_directories(tq PRIVATE "third-party")
target_compile_definitions(tq PRIVATE TQ_BUILD)

#-------------------------------------------------------------------------------
# Dependencies

# Check if pkg-config is available. It will be used as a fallback.
find_package(PkgConfig MODULE)

#-----------------------------------------------------------
# Standard math library

find_library(MATH_LIB NAMES m)

if(MATH_LIB)
    target_link_libraries(tq PUBLIC ${MATH_LIB})
endif()

#-----------------------------------------------------------
# Threads

find_package(Threads MODULE REQUIRED)
target_link_libraries(tq PRIVATE Threads::Threads)

#-----------------------------------------------------------
# SDL 2

if(TQ_PLATFORM STREQUAL "Emscripten" OR TQ_PLATFORM STREQUAL "Linux")
    find_package(SDL2 CONFIG REQUIRED) # Imports SDL2::SDL2
    target_link_libraries(tq PRIVATE SDL2::SDL2)
endif()

#-----------------------------------------------------------
# Android system libraries

if(TQ_PLATFORM STREQUAL "Android")
    target_link_libraries(tq PRIVATE android log)
endif()

#-----------------------------------------------------------
# OpenGL / OpenGL ES

if(TQ_PLATFORM STREQUAL "Emscripten")
    target_link_options(tq PUBLIC -sFULL_ES2=1 -sFULL_ES3=1)
elseif(TQ_PLATFORM STREQUAL "Android")
    target_link_libraries(tq PRIVATE GLESv2 EGL)
else()
    find_package(OpenGL MODULE REQUIRED)
    find_package(GLEW MODULE REQUIRED)

    target_link_libraries(tq PRIVATE OpenGL::GL GLEW::GLEW)

    if(TQ_PLATFORM STREQUAL "Linux" AND USE_GLES2)
        target_link_libraries(tq PRIVATE GLESv2) # should be ok?
    endif()
endif()

#-----------------------------------------------------------
# FreeType

if(TQ_PLATFORM STREQUAL "Emscripten")
    target_compile_options(tq PRIVATE -sUSE_FREETYPE=1)
    target_link_libraries(tq PRIVATE freetype)
else()
    find_package(Freetype MODULE REQUIRED)
    target_link_libraries(tq PRIVATE Freetype::Freetype)
endif()

#-----------------------------------------------------------
# HarfBuzz

if(USE_HARFBUZZ)
    if(TQ_PLATFORM STREQUAL "Emscripten")
        target_compile_options(tq PRIVATE -sUSE_HARFBUZZ=1)
        target_link_libraries(tq PRIVATE harfbuzz)
    else()
        find_package(harfbuzz CONFIG REQUIRED)
        target_link_libraries(tq PRIVATE harfbuzz::harfbuzz)
    endif()
endif()

#-----------------------------------------------------------
# OpenAL

if(TQ_PLATFORM STREQUAL "Win32" OR TQ_PLATFORM STREQUAL "Linux")
    find_package(OpenAL CONFIG QUIET)

    if(OpenAL_FOUND)
        target_link_libraries(tq PRIVATE OpenAL::OpenAL)
    elseif(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_openal REQUIRED IMPORTED_TARGET openal)
        target_link_libraries(tq PRIVATE PkgConfig::PC_openal)
    else()
        message(FATAL_ERROR "OpenAL is required, but not found.")
    endif()
endif()

#-----------------------------------------------------------
# Vorbis

if(TQ_PLATFORM STREQUAL "Emscripten")
    target_compile_options(tq PRIVATE -sUSE_VORBIS=1)
    target_link_libraries(tq PRIVATE vorbis ogg)
else()
    # Imports ::vorbisenc and ::vorbisfile
    find_package(Vorbis CONFIG QUIET)

    if(Vorbis_FOUND)
        target_compile_definitions(tq PRIVATE TQ_USE_OGG)
        target_link_libraries(tq PRIVATE Vorbis::vorbisfile)
    elseif(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_vorbisfile IMPORTED_TARGET vorbisfile)

        if(PC_vorbisfile_FOUND)
            target_compile_definitions(tq PRIVATE TQ_USE_OGG)
            target_link_libraries(tq PRIVATE PkgConfig::PC_vorbisfile)
        endif()
    endif()
endif()

#-----------------------------------------------------------
# ASAN

# Enable AddressSanitizer for debug build
if(NOT MSVC)
    target_compile_options(tq PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    target_link_libraries(tq PUBLIC $<$<CONFIG:Debug>:-fsanitize=address>)
endif()

#-----------------------------------------------------------
# WinMain handler

if(TQ_PLATFORM STREQUAL "Win32")
    if(BUILD_SHARED_LIBS)
        add_library(tq-main STATIC "src/tq_win32_system.c")
        target_compile_definitions(tq-main PRIVATE TQ_WIN32)
        target_link_libraries(tq INTERFACE tq-main)
    else()
        target_sources(tq PUBLIC "src/tq_win32_system.c")
    endif()
endif()

#-------------------------------------------------------------------------------
# Compile definitions

#-----------------------------------------------------------
# Platform

if(BUILD_SHARED_LIBS)
    target_compile_definitions(tq PRIVATE TQ_SHARED)
endif()

if(TQ_PLATFORM STREQUAL "Emscripten")
    target_compile_definitions(tq PRIVATE TQ_EMSCRIPTEN)
elseif(TQ_PLATFORM STREQUAL "Android")
    target_compile_definitions(tq PRIVATE TQ_ANDROID)
elseif(TQ_PLATFORM STREQUAL "Win32")
    target_compile_definitions(tq PRIVATE TQ_WIN32)
elseif(TQ_PLATFORM STREQUAL "Linux")
    target_compile_definitions(tq PRIVATE TQ_LINUX)
endif()

#-----------------------------------------------------------
# Dependencies

if(USE_GLES2 OR TQ_PLATFORM STREQUAL "Emscripten")
    target_compile_definitions(tq PRIVATE TQ_USE_GLES2)
endif()

if (USE_HARFBUZZ)
    target_compile_definitions(tq PRIVATE TQ_USE_HARFBUZZ)
endif()

#-------------------------------------------------------------------------------
# Examples

if(EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX ".html")
    set(EXAMPLES
        "emscripten")
else()
    set(EXAMPLES
        "audio-music"
        "core-hello"
        "core-keyboard"
        "core-mouse"
        "graphics-textures"
        "graphics-surfaces"
        "graphics-text"
        "graphics-blend")
endif()

if(BUILD_EXAMPLES)
    # Copy assets to build directory
    # TODO: move assets directory to examples/assets
    add_custom_command(TARGET tq POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:tq>/assets)

    foreach(EXAMPLE ${EXAMPLES})
        add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
        target_link_libraries(${EXAMPLE} tq)
        set_target_properties(${EXAMPLE} PROPERTIES WIN32_EXECUTABLE YES)
    endforeach()
endif()

#-------------------------------------------------------------------------------
# Installation

include(GNUInstallDirs)

install(TARGETS tq)
install(DIRECTORY "include/tq" TYPE INCLUDE)

if(WIN32 AND BUILD_SHARED_LIBS)
    install(TARGETS tq-main)
endif()

# -- Support example installation for Windows only
if(WIN32 AND BUILD_EXAMPLES)
    install(DIRECTORY "assets" DESTINATION "examples")

    foreach(EXAMPLE ${EXAMPLES})
        install(FILES "examples/${EXAMPLE}.c" DESTINATION "examples/src")
        install(TARGETS ${EXAMPLE} DESTINATION "examples")
    endforeach()

    # -- Put DLL to examples directory
    if(BUILD_SHARED_LIBS)
        install(TARGETS tq DESTINATION "examples")
    endif()
endif()
