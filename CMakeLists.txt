
cmake_minimum_required(VERSION 3.13)
project(tq)

#---------------------------------------
# Library target
#---------------------------------------

add_library(tq
    "src/tq_api.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_gl_renderer.c"
    "src/tq_graphics.c"
    "src/tq_file.c"
    "src/tq_handle_list.c"
    "src/tq_image_loader.c"
    "src/tq_log.c"
    "src/tq_math.c"
    "src/tq_meta.c"
    "src/tq_sf_display.cpp"
    "src/tq_sf_clock.cpp"
    "src/tq_sf_mixer.cpp"
    "src/tq_stream.c")

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

function(USE_SFML)
    find_package(SFML 2.5 REQUIRED COMPONENTS audio window)
    target_compile_definitions(tq PRIVATE TQ_USE_SFML)
    target_link_libraries(tq sfml-audio sfml-window)
endfunction()

function(USE_OPENGL)
    find_package(OpenGL REQUIRED)
    find_package(GLEW REQUIRED)
    target_compile_definitions(tq PRIVATE TQ_USE_OPENGL)
    target_link_libraries(tq GLEW::GLEW OpenGL::GL)
endfunction()

function(USE_OPENGL_ES)
    find_library(GLESv2_LIBRARY "GLESv2" REQUIRED)
    target_compile_definitions(tq PRIVATE TQ_USE_OPENGL TQ_USE_OPENGL_ES)
    target_link_libraries(tq ${GLESv2_LIBRARY})
endfunction()

if(WIN32)
    #-----------------------------------
    # Windows

    use_sfml()
    use_opengl()

    install(TARGETS tq DESTINATION "lib")
    install(FILES
        "include/tq/tq.h"
        "include/tq/tq.hpp"
        DESTINATION "include")
elseif(UNIX AND NOT APPLE)
    #-----------------------------------
    # Linux

    use_sfml()
    use_opengl()
    target_link_libraries(tq m)

    include(GNUInstallDirs)
    install(TARGETS tq DESTINATION ${CMAKE_INSTALL_LIBDIR})
    install(FILES
        "include/tq/tq.h"
        "include/tq/tq.hpp"
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
else()
    # Unsupported OS
    message(FATAL_ERROR "Sorry, but your OS is not supported :(")
endif()

#-------------------------------------------------------------------------------
# Examples

if(BUILD_EXAMPLES)
    set(EXAMPLES "hello" "defense" "jestan")

    foreach(EXAMPLE ${EXAMPLES})
        add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
        target_link_libraries(${EXAMPLE} tq)
    endforeach()
endif()
