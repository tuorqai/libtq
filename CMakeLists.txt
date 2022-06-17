
cmake_minimum_required(VERSION 3.13)
project(tq)

add_library(tq
    "src/tq_api.c"
    "src/tq_audio.c"
    "src/tq_core.c"
    "src/tq_graphics.c"
    "src/tq_file.c"
    "src/tq_handle_list.c"
    "src/tq_log.c"
    "src/tq_meta.c"
    "src/tq_sf_display.cpp"
    "src/tq_sf_clock.cpp"
    "src/tq_sf_mixer.cpp"
    "src/tq_sf_renderer.cpp"
    "src/tq_stream.c")

target_include_directories(tq PUBLIC "include")

target_compile_definitions(tq PRIVATE TQ_BUILD)
target_compile_definitions(tq PRIVATE TQ_USE_SFML)

set_target_properties(tq PROPERTIES
    C_VISIBILITY_PRESET hidden
    CXX_VISIBILITY_PRESET hidden)

find_package(SFML 2.5 REQUIRED COMPONENTS audio graphics)

target_link_libraries(tq sfml-graphics sfml-audio m)

#-------------------------------------------------------------------------------
# Examples

set(EXAMPLES "hello" "defense")

foreach(EXAMPLE ${EXAMPLES})
    add_executable(${EXAMPLE} "examples/${EXAMPLE}.c")
    target_link_libraries(${EXAMPLE} tq)
endforeach()
