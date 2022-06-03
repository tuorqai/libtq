
cmake_minimum_required(VERSION 3.13)
project(tq)

add_library(tq
    "src/tq_api.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_graphics.c"
    "src/tq_file.c"
    "src/tq_handle_list.c"
    "src/tq_meta.c"
    "src/tq_sf2_clock.c"
    "src/tq_sf2_display_and_renderer.c"
    "src/tq_sf2_mixer.c")

target_include_directories(tq PUBLIC "include")

target_compile_definitions(tq PRIVATE TQ_BUILD)
target_compile_definitions(tq PRIVATE TQ_USE_SFML)

set_target_properties(tq PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden)

find_package(SFML 2.5 REQUIRED COMPONENTS audio graphics)

find_library(LIB_MATH "m")

if(LIB_MATH)
    target_link_libraries(tq ${LIB_MATH})
endif()

find_library(LIB_CSFML_SYSTEM "csfml-system" REQUIRED)
find_library(LIB_CSFML_WINDOW "csfml-window" REQUIRED)
find_library(LIB_CSFML_GRAPHICS "csfml-graphics" REQUIRED)
find_library(LIB_CSFML_AUDIO "csfml-audio" REQUIRED)

target_link_libraries(tq
    ${LIB_CSFML_SYSTEM}
    ${LIB_CSFML_WINDOW}
    ${LIB_CSFML_GRAPHICS}
    ${LIB_CSFML_AUDIO})

#-------------------------------------------------------------------------------
# Examples

set(EXAMPLES "hello")

foreach(EXAMPLE ${EXAMPLES})
    add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
    target_link_libraries(${EXAMPLE} tq)
endforeach()
