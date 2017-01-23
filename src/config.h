#ifndef THEREMIN_CONFIG_H
#define THEREMIN_CONFIG_H

#include "const.h"

// "true" to show a real-time graphical info display
// which might be too expensive for some computers to
// maintain in addition to the audio synthesis
#define REALTIME_DISPLAY true

// The default waveform, see WAVE_* inside const.h
#define WAVEFORM WAVE_SIN

// Lowest possible note to play
// One of the notes defined in const.h
#define LOWEST_NOTE E_0

// Amount of octaves from the lowest note upwards
// that can be played (excluding pedal effect)
// Must be a low positive integer
#define NUM_OCTAVES 2

// "true" to output each sample on stdout 
// (WARNING: very much data, do not pipe into a file
// or it will fill your disk rather fast!)
#define LOG_DATA false

// "true" to output each new frequency to stdout
#define LOG_FREQ false

// Maximal possible volume to play at
// Valid values are 1..(2^bitdepth)-1,
// where the bitdepth is hardcoded to 16-bit unsigned
// => Valid values are 1..65535
#define MAX_VOLUME 65535

// The input device (see INPUT_* inside const.h)
#define INPUT_DEVICE INPUT_DEVICE_MOUSE

// The default autotune mode
// See AUTOTUNE_* inside const.h
#define AUTOTUNE_MODE AUTOTUNE_SMOOTH

// The amount of generated and played samples per second.
// Can be tweaked downwards to get better performance but
// worse audio quality
#define AUDIO_SAMPLE_RATE 16384

// Size of the application's audio buffer.
// Higher values should mean 
#define AUDIO_BUFFER_SIZE 512

// Determines by how many ticks the volume will be decreased
// each time there is no valid sensor data for it
// Must be a floating-point number
#define MAX_VOLUME_CHANGE_PER_TICK 30.0

// "true" to enable tremolo by default
#define TREMOLO_ENABLED false

// The tremolo intensity
// Must be a floating-point number between 0.0 and 0.5
#define TREMOLO_INTENSITY 0.3

// The frequency of tremolo (i.e. how often
// the volume is being modulated per second)
// Must be greater than zero
#define TREMOLO_FREQUENCY 7

// The triggerable actions
// An array of values in the form ACTION_*
// without duplicates
constexpr static const int ACTIONS[] = {
    ACTION_SUSTAINED_NOTE, 
    ACTION_OCTAVE_UP, 
    ACTION_TREMOLO, 
    ACTION_AUTOTUNE_NONE, 
    ACTION_AUTOTUNE_SMOOTH, 
    ACTION_AUTOTUNE_FULL,
    ACTION_CHANGE_WAVEFORM
};

// The inputs to be mapped to the corresponding
// action (see ACTIONS)
// An array of SDL keycodes of the length NUM_INPUTS
constexpr const static int INPUTS[] = {
    SDLK_a, 
    SDLK_b, 
    SDLK_c, 
    SDLK_1, 
    SDLK_2, 
    SDLK_3,
    SDLK_w
};

// The length of the array INPUTS
#define NUM_INPUTS 7

// Frequencies of specific application tasks;
// each number means that the corresponding task is done 
// about this many times per second, as long as the 
// audio runs smoothly without stagnating.
#define TASK_FREQUENCY_INPUT_MOUSE 100
#define TASK_FREQUENCY_INPUT_SENSOR 100
#define TASK_FREQUENCY_INPUT_GENERAL 50
#define TASK_FREQUENCY_DISPLAY_REFRESH 25

// Calculates the sample period for the tasks
// (Do not change this, change TASK_FREQUENCY_* instead)
#define PERIOD_INPUT_MOUSE (AUDIO_SAMPLE_RATE/TASK_FREQUENCY_INPUT_MOUSE)
#define PERIOD_INPUT_SENSOR (AUDIO_SAMPLE_RATE/TASK_FREQUENCY_INPUT_SENSOR)
#define PERIOD_INPUT_GENERAL (AUDIO_SAMPLE_RATE/TASK_FREQUENCY_INPUT_GENERAL)
#define PERIOD_DISPLAY_REFRESH (AUDIO_SAMPLE_RATE/TASK_FREQUENCY_DISPLAY_REFRESH)

#endif
