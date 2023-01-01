
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.14)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

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
# Detect target platform

if(EMSCRIPTEN)
    set(TQ_EMSCRIPTEN ON)
elseif(ANDROID)
    set(TQ_ANDROID ON)
elseif(WIN32)
    set(TQ_WIN32 ON)
elseif(UNIX)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        set(TQ_LINUX ON)
    else()
        message(FATAL_ERROR "Unsupported Unix system: ${CMAKE_SYSTEM_NAME}.")
    endif()

    set(TQ_UNIX ON)
else()
    message(FATAL_ERROR "Sorry, but your platform is out of luck.")
endif()

#-------------------------------------------------------------------------------
# Declare dependencies

include(FetchContent)

FetchContent_Declare(sdl2
    URL             https://github.com/libsdl-org/SDL/releases/download/release-2.26.1/SDL2-2.26.1.tar.gz
    URL_HASH        SHA256=02537cc7ebd74071631038b237ec4bfbb3f4830ba019e569434da33f42373e04)

FetchContent_Declare(glew
    URL             https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.tgz
    URL_HASH        SHA256=04de91e7e6763039bc11940095cd9c7f880baba82196a7765f727ac05a993c95
    SOURCE_SUBDIR   build/cmake)

FetchContent_Declare(freetype
    URL             https://download.savannah.gnu.org/releases/freetype/freetype-2.12.1.tar.gz
    URL_HASH        SHA256=efe71fd4b8246f1b0b1b9bfca13cfff1c9ad85930340c27df469733bbb620938)

FetchContent_Declare(harfbuzz
    URL             https://github.com/harfbuzz/harfbuzz/releases/download/5.3.1/harfbuzz-5.3.1.tar.xz
    URL_HASH        SHA256=4a6ce097b75a8121facc4ba83b5b083bfec657f45b003cd5a3424f2ae6b4434d)

FetchContent_Declare(openal
    URL             https://openal-soft.org/openal-releases/openal-soft-1.22.2.tar.bz2
    URL_HASH        SHA256=ae94cc95cda76b7cc6e92e38c2531af82148e76d3d88ce996e2928a1ea7c3d20)

FetchContent_Declare(libogg
    URL             https://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.gz
    URL_HASH        SHA256=0eb4b4b9420a0f51db142ba3f9c64b333f826532dc0f48c6410ae51f4799b664)

FetchContent_Declare(libvorbis
    URL             https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.7.tar.xz
    URL_HASH        SHA256=b33cc4934322bcbf6efcbacf49e3ca01aadbea4114ec9589d1b1e9d20f72954b)

#-------------------------------------------------------------------------------
# Options

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(TQ_BUILD_EXAMPLES "Build examples" OFF)
option(TQ_USE_HARFBUZZ "Enable HarfBuzz (recommended)" ON)
option(TQ_USE_OGG "Enable Ogg Vorbis decoder" ON)

if(TQ_LINUX)
    option(TQ_USE_GLES2 "Force OpenGL ES 2.0 usage on Linux" OFF)
endif()

#-------------------------------------------------------------------------------

set(CMAKE_POSITION_INDEPENDENT_CODE YES)

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

set_target_properties(tq PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    C_VISIBILITY_PRESET hidden)

target_compile_definitions(tq
    PRIVATE
        TQ_BUILD
        $<$<BOOL:${BUILD_SHARED_LIBS}>:TQ_SHARED>
        $<$<BOOL:${TQ_UNIX}>:TQ_UNIX>
        $<$<BOOL:${TQ_LINUX}>:TQ_LINUX>
        $<$<BOOL:${TQ_WIN32}>:TQ_WIN32>
        $<$<BOOL:${TQ_EMSCRIPTEN}>:TQ_EMSCRIPTEN>
        $<$<BOOL:${TQ_ANDROID}>:TQ_ANDROID>
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

if(NOT MSVC)
    target_compile_options(tq
        PUBLIC
            -Wall
            -Wextra
            -Wno-switch
            -Wno-switch-default
            -Wno-unused-parameter
            -Wno-unused-variable)
endif()

target_include_directories(tq
    PUBLIC
        include
    PRIVATE
        third-party)

#-------------------------------------------------------------------------------
# Dependencies

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

if(TQ_UNIX OR TQ_EMSCRIPTEN)
    find_package(SDL2 CONFIG 2.0)

    if(NOT SDL2_FOUND)
        FetchContent_MakeAvailable(sdl2)
    endif()

    target_link_libraries(tq PRIVATE SDL2::SDL2)
endif()

#-----------------------------------------------------------
# Android system libraries

if(TQ_ANDROID)
    target_link_libraries(tq PRIVATE android log)
endif()

#-----------------------------------------------------------
# OpenGL / OpenGL ES

if(TQ_EMSCRIPTEN)
    target_link_options(tq PUBLIC -sFULL_ES2=1 -sFULL_ES3=1)
elseif(TQ_ANDROID)
    target_link_libraries(tq PRIVATE GLESv2 EGL)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(tq PRIVATE OpenGL::GL)

    if(TQ_USE_GLES2)
        target_compile_definitions(tq PRIVATE TQ_USE_GLES2)
        target_link_libraries(tq PRIVATE GLESv2) # should be ok?
    endif()
endif()

#-----------------------------------------------------------
# GLEW

if(TQ_UNIX OR TQ_WIN32)
    find_package(GLEW)

    if(GLEW_FOUND)
        target_link_libraries(tq PRIVATE GLEW::GLEW)
    else()
        FetchContent_MakeAvailable(glew)

        target_include_directories(tq PRIVATE ${glew_SOURCE_DIR}/include)
        target_link_libraries(tq PRIVATE glew_s)
    endif()
endif()

#-----------------------------------------------------------
# FreeType

find_package(Freetype 2.6)

if(FREETYPE_FOUND)
    target_link_libraries(tq PRIVATE Freetype::Freetype)
else()
    FetchContent_MakeAvailable(freetype)
    target_link_libraries(tq PRIVATE freetype)
endif()

#-----------------------------------------------------------
# HarfBuzz

if(TQ_USE_HARFBUZZ)
    find_package(harfbuzz)

    if(harfbuzz_FOUND)
        target_link_libraries(tq PRIVATE harfbuzz::harfbuzz)
    else()
        FetchContent_MakeAvailable(harfbuzz)
        target_link_libraries(tq PRIVATE harfbuzz)
    endif()

    target_compile_definitions(tq PRIVATE TQ_USE_HARFBUZZ)
endif()

#-----------------------------------------------------------
# OpenAL

if(TQ_UNIX OR TQ_WIN32)
    find_package(OpenAL)

    if(OPENAL_FOUND)
        target_include_directories(tq PRIVATE ${OPENAL_INCLUDE_DIR})
        target_link_libraries(tq PRIVATE ${OPENAL_LIBRARY})
    else()
        set(ALSOFT_UTILS OFF CACHE INTERNAL "...")
        set(ALSOFT_EXAMPLES OFF CACHE INTERNAL "...")

        FetchContent_MakeAvailable(openal)
        target_link_libraries(tq PRIVATE OpenAL)
    endif()
endif()

#-----------------------------------------------------------
# Vorbis

if(TQ_USE_OGG)
    # libogg is a transient dependency of libvorbis.

    FetchContent_MakeAvailable(libogg)

    set(OGG_INCLUDE_DIR ${libogg_SOURCE_DIR}/include ${libogg_BINARY_DIR}/include)
    set(OGG_LIBRARY $<TARGET_FILE:ogg>)

    FetchContent_MakeAvailable(libvorbis)

    target_compile_definitions(tq PRIVATE TQ_USE_OGG)
    target_link_libraries(tq PRIVATE vorbisfile)
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

if(TQ_WIN32)
    if(BUILD_SHARED_LIBS)
        add_library(tq-main STATIC "src/tq_win32_system.c")
        target_compile_definitions(tq-main PRIVATE TQ_WIN32)
        target_link_libraries(tq INTERFACE tq-main)
    else()
        target_sources(tq PUBLIC "src/tq_win32_system.c")
    endif()
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

if(TQ_BUILD_EXAMPLES)
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
if(WIN32 AND TQ_BUILD_EXAMPLES)
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
