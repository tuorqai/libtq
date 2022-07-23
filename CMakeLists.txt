
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
    "src/tq_sound_decoder.c"
    "src/tq_stream.c"
    "src/tq_text.c"
    "src/tq_win32_threads.c")

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
    set(USE_PLATFORM_DESKTOP TRUE)
    message("Platform is: Windows")
elseif(UNIX)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        set(USE_PLATFORM_LINUX TRUE)
        set(USE_PLATFORM_UNIX TRUE)
        set(USE_PLATFORM_DESKTOP TRUE)
        message("Platform is: Linux")
    elseif(ANDROID)
        set(USE_PLATFORM_ANDROID TRUE)
        set(USE_PLATFORM_UNIX TRUE)
        message("Platform is: Android")
    endif()
endif()

if(USE_PLATFORM_WINDOWS)
    install(TARGETS tq DESTINATION "lib")
    install(FILES "include/tq/tq.h" DESTINATION "include")
    target_compile_definitions(tq PRIVATE TQ_PLATFORM_WINDOWS)
elseif(USE_PLATFORM_LINUX)
    include(GNUInstallDirs)
    install(TARGETS tq DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES "include/tq/tq.h" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    target_compile_definitions(tq PRIVATE TQ_PLATFORM_LINUX)
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

if(USE_PLATFORM_DESKTOP)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(OpenAL REQUIRED)

    target_include_directories(tq PRIVATE ${OPENAL_INCLUDE_DIR})
    target_link_libraries(tq SDL2::SDL2 GLEW::GLEW OpenGL::GL ${OPENAL_LIBRARY})
    target_compile_definitions(tq PRIVATE TQ_USE_SDL TQ_USE_OPENGL TQ_USE_OPENAL)
endif()

if(USE_PLATFORM_UNIX)
    find_library(M_LIBRARY m)

    if(M_LIBRARY)
        target_link_libraries(tq ${M_LIBRARY})
    endif()

    target_compile_definitions(tq PRIVATE TQ_PLATFORM_UNIX)
endif()

find_package(Threads REQUIRED)
target_link_libraries(tq Threads::Threads)

find_package(Freetype REQUIRED)
find_package(harfbuzz REQUIRED)
target_link_libraries(tq Freetype::Freetype harfbuzz::harfbuzz)

find_library(OGG_LIBRARY ogg)
find_library(VORBIS_LIBRARY vorbis)
find_library(VORBISFILE_LIBRARY vorbisfile)

if(OGG_LIBRARY AND VORBIS_LIBRARY AND VORBISFILE_LIBRARY)
    target_link_libraries(tq ${OGG_LIBRARY} ${VORBIS_LIBRARY} ${VORBISFILE_LIBRARY})
    target_compile_definitions(tq PRIVATE TQ_USE_OGG)
endif()

#-------------------------------------------------------------------------------
# Examples

if(BUILD_EXAMPLES)
    set(EXAMPLES
        "hello"
        "defense"
        "jestan"
        "callbacks"
        "sounds"
        "text"
        "graphics-surfaces")

    foreach(EXAMPLE ${EXAMPLES})
        add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
        target_link_libraries(${EXAMPLE} tq)
    endforeach()
endif()
