cmake_minimum_required (VERSION 2.6)
project (theresa)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -O0")
#set(ENV{THEREMIN_GUI} TRUE)

include_directories(${PROJECT_SOURCE_DIR}/tinkerforge/source/)

link_directories(${PROJECT_SOURCE_DIR}/tinkerforge/source/)

add_executable(
theresa
    src/configuration.cpp
    src/music_util.cpp
    src/wave_synth.cpp 
    src/user_interface.cpp 
    src/audio.cpp 
    src/sensor_input.cpp 
    src/main.cpp
)

if (DEFINED ENV{THEREMIN_GUI})

message("Building with GUI.")
target_link_libraries(
theresa
    SDL2
    SDL2_ttf
    tinkerforge
    config++
)
add_compile_definitions(THEREMIN_GUI)

else ()

message("Building with CLI.")
target_link_libraries(
theresa
    SDL2
    tinkerforge
    config++
    ncurses
)

endif ()
