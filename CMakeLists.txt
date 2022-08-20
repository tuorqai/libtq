
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

#-------------------------------------------------------------------------------

# Quite dirty regex to pick up version from tq.h
set(TQ_VERSION_REGEX "#define TQ_VERSION[ \t]+\"(.+)\"")

# Read a line with version #define to TQ_VERSION variable
file(STRINGS "${CMAKE_SOURCE_DIR}/include/tq/tq.h"
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
# Platforms

if(WIN32)
    set(TQ_PLATFORM_WIN32 YES)
elseif(UNIX)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(TQ_PLATFORM_LINUX YES)
    endif()
elseif(ANDROID) # is UNIX defined on Android?
    set(TQ_PLATFORM_ANDROID YES)
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
    "src/tq_graphics.c"
    "src/tq_handle_list.c"
    "src/tq_image_loader.c"
    "src/tq_log.c"
    "src/tq_math.c"
    "src/tq_meta.c"
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

if(TQ_PLATFORM_WIN32)
    target_compile_definitions(tq PRIVATE TQ_WIN32)
elseif(TQ_PLATFORM_LINUX)
    target_compile_definitions(tq PRIVATE TQ_LINUX)
elseif(TQ_PLATFORM_ANDROID)
    target_compile_definitions(tq PRIVATE TQ_ANDROID)
endif()

#-------------------------------------------------------------------------------
# Shared library support

option(BUILD_SHARED_LIBS "Build shared libraries" YES)

if(BUILD_SHARED_LIBS)
    if(ANDROID)
        message(FATAL_ERROR "Shared library build is not supported on Android.")
    endif()

    target_compile_definitions(tq PRIVATE TQ_SHARED)
endif()

#-------------------------------------------------------------------------------
# Dependencies

# Check if pkg-config is available.
# It will be used as a fallback.
find_package(PkgConfig MODULE)

# Standard math library.
find_library(MATH_LIB NAMES m)

if(MATH_LIB)
    target_link_libraries(tq PUBLIC ${MATH_LIB})
endif()

# System-dependent threading library.
# Win32 threads on Windows, pthreads otherwise.
find_package(Threads MODULE REQUIRED) # Imports `Threads::Threads` target
target_link_libraries(tq PRIVATE Threads::Threads)

# FreeType font renderer.
find_package(Freetype MODULE REQUIRED) # Imports `Freetype::Freetype` target
target_link_libraries(tq PRIVATE Freetype::Freetype)

# HarfBuzz text shaper.
find_package(harfbuzz CONFIG REQUIRED) # Imports `harfbuzz::harfbuzz` target
target_link_libraries(tq PRIVATE harfbuzz::harfbuzz)

# Vorbis library.
# This is an optional dependency.
find_package(Vorbis CONFIG QUIET) # Imports Vorbis::vorbisenc and Vorbis::vorbisfile

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

#-------------------------------------------------------------------------------
# Platform-specific dependencies

if(TQ_PLATFORM_WIN32)
    if(BUILD_SHARED_LIBS)
        add_library(tq-main STATIC "src/tq_win32_system.c")
        target_compile_definitions(tq-main PRIVATE TQ_WIN32)
        target_link_libraries(tq INTERFACE tq-main)
    else()
        target_sources(tq PUBLIC "src/tq_win32_system.c")
    endif()
endif()

if(TQ_PLATFORM_LINUX)
    # Simple DirectMedia Layer 2
    find_package(SDL2 CONFIG REQUIRED) # Imports SDL2::SDL2
    target_link_libraries(tq PRIVATE SDL2::SDL2)
endif()

if(TQ_PLATFORM_WIN32 OR TQ_PLATFORM_LINUX)
    # OpenAL
    find_package(OpenAL CONFIG QUIET) # Imports OpenAL::OpenAL

    if(OpenAL_FOUND)
        target_link_libraries(tq PRIVATE OpenAL::OpenAL)
    elseif(PKG_CONFIG_FOUND)
        pkg_check_modules(PC_openal REQUIRED IMPORTED_TARGET openal)
        target_link_libraries(tq PRIVATE PkgConfig::PC_openal)
    else()
        message(FATAL_ERROR "OpenAL is required, but not found.")
    endif()

    # OpenGL
    find_package(OpenGL MODULE REQUIRED) # Imports OpenGL::GL and others
    target_link_libraries(tq PRIVATE OpenGL::GL)

    # OpenGL Extension Wrangler
    find_package(GLEW MODULE REQUIRED) # Imports GLEW::GLEW
    target_link_libraries(tq PRIVATE GLEW::GLEW)
endif()

if(TQ_PLATFORM_ANDROID)
    # -landroid
    find_library(ANDROID_LIB NAMES android)

    # -llog
    find_library(LOG_LIB NAMES log)

    # -lGLESv2
    find_library(GLESV2_LIB NAMES GLESv2)

    # -lEGL
    find_library(EGL_LIB NAMES EGL)

    target_link_libraries(tq PRIVATE
        ${ANDROID_LIB}
        ${LOG_LIB}
        ${GLESV2_LIB}
        ${EGL_LIB})
endif()

#-------------------------------------------------------------------------------
# Examples

set(EXAMPLES
    "audio-music"
    "core-hello"
    "core-keyboard"
    "core-mouse"
    "graphics-textures"
    "graphics-surfaces"
    "graphics-text"
    "graphics-blend")

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
