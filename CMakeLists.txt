cmake_minimum_required (VERSION 2.6)
project (theremin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${PROJECT_SOURCE_DIR}/tinkerforge/source/)

link_directories(${PROJECT_SOURCE_DIR}/tinkerforge/source/)

add_executable(
theremin 
    src/wave_synth.cpp 
    src/input.cpp 
    src/audio.cpp 
    src/sensor_input.cpp 
    src/main.cpp
)

target_link_libraries(
theremin
    SDL2
    SDL2_ttf
    tinkerforge
)
