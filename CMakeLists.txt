
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
# Options

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(TQ_BUILD_EXAMPLES "Build examples" OFF)
option(TQ_USE_HARFBUZZ "Enable HarfBuzz (recommended)" ON)
option(TQ_USE_OGG "Enable Ogg Vorbis decoder" ON)

if(UNIX)
    option(TQ_USE_GLES2 "Force OpenGL ES 2.0 usage" OFF)
endif()

#-------------------------------------------------------------------------------

set(CMAKE_POSITION_INDEPENDENT_CODE YES)

#-------------------------------------------------------------------------------
# tq target and its dependencies

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
        $<$<C_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>
        $<$<BOOL:${TQ_USE_HARFBUZZ}>:TQ_USE_HARFBUZZ>
        $<$<BOOL:${TQ_USE_OGG}>:TQ_USE_OGG>
        $<$<BOOL:${TQ_USE_GLES2}>:TQ_USE_GLES2>)

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

include(FetchContent)

find_library(MATH_LIB NAMES m)
find_package(Threads MODULE REQUIRED)
find_package(Freetype 2.6)

if(NOT FREETYPE_FOUND)
    FetchContent_Declare(freetype
        URL https://download.savannah.gnu.org/releases/freetype/freetype-2.12.1.tar.gz
        URL_HASH SHA256=efe71fd4b8246f1b0b1b9bfca13cfff1c9ad85930340c27df469733bbb620938)
    FetchContent_MakeAvailable(freetype)

    add_library(Freetype::Freetype ALIAS freetype)
endif()

if(TQ_USE_HARFBUZZ)
    find_package(harfbuzz)

    if(NOT harfbuzz_FOUND)
        set(HB_HAVE_FREETYPE ON CACHE INTERNAL "Enable FreeType for Harfbuzz")
        set(HB_BUILD_SUBSET OFF CACHE INTERNAL "Build hb-subset")

        FetchContent_Declare(harfbuzz
            URL https://github.com/harfbuzz/harfbuzz/releases/download/5.3.1/harfbuzz-5.3.1.tar.xz
            URL_HASH SHA256=4a6ce097b75a8121facc4ba83b5b083bfec657f45b003cd5a3424f2ae6b4434d)
        FetchContent_MakeAvailable(harfbuzz)

        add_library(harfbuzz::harfbuzz ALIAS harfbuzz)
    endif()
endif()

if(TQ_USE_OGG)
    # libogg is a transient dependency of libvorbis.

    FetchContent_Declare(libogg
        URL https://downloads.xiph.org/releases/ogg/libogg-1.3.5.tar.gz
        URL_HASH SHA256=0eb4b4b9420a0f51db142ba3f9c64b333f826532dc0f48c6410ae51f4799b664)
    FetchContent_MakeAvailable(libogg)

    set(OGG_INCLUDE_DIR ${libogg_SOURCE_DIR}/include ${libogg_BINARY_DIR}/include)
    set(OGG_LIBRARY $<TARGET_FILE:ogg>)

    FetchContent_Declare(libvorbis
        URL https://downloads.xiph.org/releases/vorbis/libvorbis-1.3.7.tar.xz
        URL_HASH SHA256=b33cc4934322bcbf6efcbacf49e3ca01aadbea4114ec9589d1b1e9d20f72954b)
    FetchContent_MakeAvailable(libvorbis)
endif()

if(WIN32 OR UNIX)
    find_package(OpenGL REQUIRED)
    find_package(GLEW)
    find_package(OpenAL)

    if(GLEW_FOUND)
        add_library(GLEW INTERFACE)
        target_include_directories(GLEW INTERFACE ${GLEW_INCLUDE_DIRS})
        target_link_libraries(GLEW INTERFACE ${GLEW_LIBRARIES})
    else()
        set(BUILD_UTILS OFF CACHE INTERNAL "Build GLEW utils")

        FetchContent_Declare(glew
            URL https://downloads.sourceforge.net/project/glew/glew/2.1.0/glew-2.1.0.tgz
            URL_HASH SHA256=04de91e7e6763039bc11940095cd9c7f880baba82196a7765f727ac05a993c95
            SOURCE_SUBDIR build/cmake)
        FetchContent_MakeAvailable(glew)

        add_library(GLEW INTERFACE)
        target_include_directories(GLEW INTERFACE ${glew_SOURCE_DIR}/include)
        target_link_libraries(GLEW INTERFACE glew_s)
    endif()

    if(OPENAL_FOUND)
        add_library(OpenAL INTERFACE)
        target_include_directories(OpenAL INTERFACE ${OPENAL_INCLUDE_DIR})
        target_link_libraries(OpenAL INTERFACE ${OPENAL_LIBRARY})
    else()
        set(ALSOFT_UTILS OFF CACHE INTERNAL "Enable OpenAL utils")
        set(ALSOFT_EXAMPLES OFF CACHE INTERNAL "Enable OpenAL examples")

        FetchContent_Declare(openal
            URL https://openal-soft.org/openal-releases/openal-soft-1.22.2.tar.bz2
            URL_HASH SHA256=ae94cc95cda76b7cc6e92e38c2531af82148e76d3d88ce996e2928a1ea7c3d20)
        FetchContent_MakeAvailable(openal)
    endif()
endif()

target_link_libraries(tq
    PUBLIC
        $<$<BOOL:${MATH_LIB}>:${MATH_LIB}>
    PRIVATE
        Threads::Threads
        Freetype::Freetype
        $<$<BOOL:${TQ_USE_HARFBUZZ}>:harfbuzz::harfbuzz>
        $<$<BOOL:${TQ_USE_OGG}>:vorbisfile>)

if(EMSCRIPTEN)
    target_compile_definitions(tq PRIVATE TQ_EMSCRIPTEN)
    target_compile_options(tq PUBLIC -sUSE_SDL=2)
    target_link_options(tq PUBLIC -sUSE_SDL=2 -sFULL_ES2=1 -sFULL_ES3=1)
elseif(ANDROID)
    target_compile_definitions(tq PRIVATE TQ_ANDROID)
    target_link_libraries(tq PRIVATE android log GLESv2 EGL)
elseif(WIN32)
    target_compile_definitions(tq PRIVATE TQ_WIN32)
    target_link_libraries(tq PRIVATE Threads::Threads OpenGL::GL GLEW OpenAL)
elseif(UNIX)
    find_package(SDL2)

    if(NOT SDL2_FOUND)
        FetchContent_Declare(sdl2
            URL https://github.com/libsdl-org/SDL/releases/download/release-2.26.1/SDL2-2.26.1.tar.gz
            URL_HASH SHA256=02537cc7ebd74071631038b237ec4bfbb3f4830ba019e569434da33f42373e04)
        FetchContent_MakeAvailable(sdl2)
    endif()

    target_compile_definitions(tq PRIVATE TQ_UNIX)

    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        target_compile_definitions(tq PRIVATE TQ_LINUX)
    endif()

    target_link_libraries(tq
        PRIVATE
            Threads::Threads
            SDL2::SDL2
            OpenGL::GL
            $<$<BOOL:${TQ_USE_GLES2}>:GLESv2>
            GLEW
            OpenAL)
else()
    message(FATAL_ERROR "Sorry, but your platform is out of luck.")
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

if(WIN32)
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

if(TQ_BUILD_EXAMPLES)
    set(EXAMPLES
        "audio-music"
        "core-hello"
        "core-keyboard"
        "core-mouse"
        "graphics-textures"
        "graphics-surfaces"
        "graphics-text"
        "graphics-blend")

    # Copy assets to build directory
    # TODO: move assets directory to examples/assets
    add_custom_command(TARGET tq POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:tq>/assets)

    if(EMSCRIPTEN)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
    endif()

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
