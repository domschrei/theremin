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
// (warning: much data, do not pipe into a file
// or it will fill your disk rather fast!)
#define LOG_DATA false

// "true" to output each new frequency to stdout
#define LOG_FREQ false

// "true" to show a real-time graphical info display
// which is too expensive for some computers to
// maintain in addition to the audio synthesis
#define REALTIME_DISPLAY true

// The default waveform
// Number from 1 to 10, see WAVE_* inside const.h
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
#define AUDIO_BUFFER_ADVERTISED_READ_SIZE 1024

// The input device (see INPUT_* inside const.h)
// Currently, only sensor works!
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

#endif
