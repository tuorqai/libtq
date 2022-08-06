
cmake_minimum_required(VERSION 3.13)
project(tq)

#---------------------------------------
# Compiler settings
#---------------------------------------

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(
        -pedantic
        -Wall
        -Wextra
        -Wno-switch
        -Wno-switch-default
        -Wno-unused-parameter
        -Wno-unused-variable)
endif()

#---------------------------------------
# Library target
#---------------------------------------

add_library(tq
    "src/tq.c"
    "src/tq_al_audio.c"
    "src/tq_android_display.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_error.c"
    "src/tq_gl_renderer.c"
    "src/tq_graphics.c"
    "src/tq_handle_list.c"
    "src/tq_image_loader.c"
    "src/tq_log.c"
    "src/tq_math.c"
    "src/tq_null_audio.c"
    "src/tq_posix_clock.c"
    "src/tq_posix_threads.c"
    "src/tq_sdl_clock.c"
    "src/tq_sdl_display.c"
    "src/tq_sound_decoder.c"
    "src/tq_stream.c"
    "src/tq_text.c"
    "src/tq_win32_threads.c"
    "src/tq_win32_system.c")

target_include_directories(tq PUBLIC "include")
target_include_directories(tq PRIVATE "third-party")
target_compile_definitions(tq PRIVATE TQ_BUILD)

#---------------------------------------
# Symbols should be exported explicitly
#---------------------------------------

set_target_properties(tq PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden)

#---------------------------------------
# Options
#---------------------------------------

option(BUILD_SHARED_LIBS "Build shared libraries" NO)
option(BUILD_EXAMPLES "Build examples" NO)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(tq PRIVATE TQ_SHARED)
endif()

#---------------------------------------
# Platform-specific
#---------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(ANDROID)
    set(APP_GLUE_PATH "${ANDROID_NDK}/sources/android/native_app_glue")
    add_library(native_app_glue STATIC "${APP_GLUE_PATH}/android_native_app_glue.c")
    target_link_libraries(tq android log native_app_glue GLESv2 EGL)
    target_compile_definitions(tq PRIVATE
        TQ_PLATFORM_ANDROID
        TQ_USE_OPENGL
        TQ_USE_OPENGL_ES)

    add_subdirectory("android-bundle")
elseif(WIN32 OR UNIX AND NOT APPLE)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenAL REQUIRED)

    target_include_directories(tq PRIVATE ${OPENAL_INCLUDE_DIR})
    target_link_libraries(tq SDL2::SDL2 OpenGL::GL GLEW::GLEW ${OPENAL_LIBRARY})
    target_compile_definitions(tq PRIVATE
        TQ_PLATFORM_DESKTOP
        TQ_USE_SDL
        TQ_USE_OPENGL
        TQ_USE_OPENAL)
else()
    message(FATAL_ERROR "Sorry, but your OS is not supported :(")
endif()

find_package(Threads REQUIRED)
find_package(FreeType REQUIRED)
find_package(HarfBuzz REQUIRED)
find_package(VorbisFile)

if(VorbisFile_FOUND)
    target_compile_definitions(tq PRIVATE TQ_USE_OGG)
endif()

target_link_libraries(tq
    Threads::Threads
    FreeType::FreeType
    HarfBuzz::HarfBuzz
    Vorbis::VorbisFile)

find_library(MATH_LIBRARY NAMES m)

if(MATH_LIBRARY)
    target_link_libraries(tq ${MATH_LIBRARY})
endif()

#-------------------------------------------------------------------------------
# Examples

if(BUILD_EXAMPLES)
    # Copy assets to build directory
    # TODO: move assets directory to examples/assets
    add_custom_command(TARGET tq POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets
            $<TARGET_FILE_DIR:tq>/assets)

    set(EXAMPLES
        "audio-music"
        "core-hello"
        "core-keyboard"
        "core-mouse"
        "graphics-textures"
        "graphics-surfaces"
        "graphics-blend")

    foreach(EXAMPLE ${EXAMPLES})
        add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
        target_link_libraries(${EXAMPLE} tq)
        set_target_properties(${EXAMPLE} PROPERTIES WIN32_EXECUTABLE YES)
    endforeach()
endif()
