
cmake_minimum_required(VERSION 3.13)
project(tq)

#---------------------------------------
# Library target
#---------------------------------------

add_library(tq
    "src/tq_al_audio.c"
    "src/tq_api.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_error.c"
    "src/tq_gl_renderer.c"
    "src/tq_graphics.c"
    "src/tq_handle_list.c"
    "src/tq_image_loader.c"
    "src/tq_log.c"
    "src/tq_math.c"
    "src/tq_posix_threads.c"
    "src/tq_sdl_clock.c"
    "src/tq_sdl_display.c"
    "src/tq_sound_loader.c"
    "src/tq_stream.c"
    "src/tq_win32_threads.c"
    "src/tq.c")

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

#---------------------------------------
# Platform-specific
#---------------------------------------

if(WIN32)
    set(USE_PLATFORM_WINDOWS TRUE)
    message("Platform is: Windows")
elseif(UNIX)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(USE_PLATFORM_LINUX TRUE)
        message("Platform is: Linux")
    elseif(ANDROID)
        set(USE_PLATFORM_ANDROID TRUE)
        message("Platform is: Android")
    endif()
endif()

find_package(Threads REQUIRED)

if(USE_PLATFORM_WINDOWS)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenAL REQUIRED)

    find_library(OGG_LIBRARY ogg REQUIRED)
    find_library(VORBIS_LIBRARY vorbis REQUIRED)
    find_library(VORBISFILE_LIBRARY vorbisfile REQUIRED)

    target_link_libraries(tq
        Threads::Threads
        SDL2::SDL2
        GLEW::GLEW
        OpenGL::GL
        ${OGG_LIBRARY}
        ${VORBIS_LIBRARY}
        ${VORBISFILE_LIBRARY})

    target_compile_definitions(tq PRIVATE
        TQ_PLATFORM_WINDOWS
        TQ_USE_SDL
        TQ_USE_OPENGL
        TQ_USE_OPENAL)

    install(TARGETS tq DESTINATION "lib")
    install(FILES "include/tq/tq.h" DESTINATION "include")
elseif(USE_PLATFORM_LINUX)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenAL REQUIRED)

    find_library(OGG_LIBRARY ogg REQUIRED)
    find_library(VORBIS_LIBRARY vorbis REQUIRED)
    find_library(VORBISFILE_LIBRARY vorbisfile REQUIRED)

    target_include_directories(tq PRIVATE ${OPENAL_INCLUDE_DIR})

    target_link_libraries(tq
        Threads::Threads
        SDL2::SDL2
        GLEW::GLEW
        OpenGL::GL
        ${OPENAL_LIBRARY}
        ${OGG_LIBRARY}
        ${VORBIS_LIBRARY}
        ${VORBISFILE_LIBRARY}
        m)

    target_compile_definitions(tq PRIVATE
        TQ_PLATFORM_LINUX
        TQ_USE_SDL
        TQ_USE_OPENGL
        TQ_USE_OPENAL)

    include(GNUInstallDirs)
    install(TARGETS tq DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES "include/tq/tq.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
elseif(USE_PLATFORM_ANDROID)
    add_library(native_app_glue STATIC
        "${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c")

    target_link_libraries(tq android log native_app_glue GLESv2 EGL)
    target_compile_definitions(tq PRIVATE
        TQ_PLATFORM_ANDROID
        TQ_USE_OPENGL
        TQ_USE_OPENGL_ES)
else()
    message(FATAL_ERROR "Sorry, but your OS is not supported :(")
endif()

#-------------------------------------------------------------------------------
# Examples

if(BUILD_EXAMPLES)
    set(EXAMPLES "hello" "defense" "jestan" "callbacks" "sounds")

    foreach(EXAMPLE ${EXAMPLES})
        add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
        target_link_libraries(${EXAMPLE} tq)
    endforeach()
endif()
