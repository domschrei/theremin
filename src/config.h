#ifndef THEREMIN_CONFIG_H
#define THEREMIN_CONFIG_H

#include "const.h"

// UID of ultrasonic distance sensor #1
#define UID_FREQUENCY "zmj" 

// UID of ultrasonic distance sensor #2
#define UID_VOLUME "zn8" 

// Host and port where to look for the brick daemon
// (usually does not have to be changed)
#define HOST "localhost"
#define PORT 4223

// "true" to output each sample on stdout 
// (WARNING: very much data, do not pipe into a file
// or it will fill your disk rather fast!)
#define LOG_DATA false

// "true" to output each new frequency to stdout
#define LOG_FREQ false

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

// Maximal possible volume to play at
// Valid values are 1..255
#define MAX_VOLUME 255

// Size of the audio ring buffer
// Must be exactly twice as large as the amount of
// samples SDL reads each time
#define AUDIO_BUFFER_SIZE 1024

// Advertised size of what SDL should read on each take
// Must be AUDIO_BUFFER_SIZE on 64-bit systems or
// AUDIO_BUFFER_SIZE / 2 on 32-bit systems
#define AUDIO_BUFFER_ADVERTISED_READ_SIZE 1024

// The amount of generated and played samples per second.
// Can be tweaked downwards to get better performance but
// worse audio quality
// WARNING: a lower sample rate also leads to lower
// polling frequencies (see PERIOD_* at the bottom).
#define AUDIO_SAMPLE_RATE 16384

// The input device (see INPUT_* inside const.h)
#define INPUT_DEVICE INPUT_DEVICE_MOUSE

// The default autotune mode
// See AUTOTUNE_* inside const.h
#define AUTOTUNE_MODE AUTOTUNE_SMOOTH

// Calibration of sensors; the MIN and MAX values
// represent lower and upper limits for the 
// distance values to be valid.
// Adjust to change the interval in which the movement
// of your hands is being recognized
#define SENSOR_FREQ_MIN_VALUE 100
#define SENSOR_FREQ_MAX_VALUE 1000
#define SENSOR_VOL_MIN_VALUE 70
#define SENSOR_VOL_MAX_VALUE 400

// Period lengths for specific application tasks;
// lower number means that the task is done 
// more frequently. At default sample rate of around 16k 
// samples/second, a value of 1000 means that the
// task is done _at least_ 16 times per second, as long
// as the audio runs smoothly without stagnating.
#define PERIOD_INPUT_MOUSE 100
#define PERIOD_INPUT_SENSOR 100
#define PERIOD_INPUT_GENERAL 250
#define PERIOD_DISPLAY_REFRESH 1000

#endif
