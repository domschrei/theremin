#ifndef THEREMIN_CONST_H
#define THEREMIN_CONST_H

/* CONFIGURATION KEYS */

#define INPUT_DEVICE "input_device"
#define REALTIME_DISPLAY "realtime_display"
#define LOG_DATA "log_data"
#define LOG_FREQ "log_freq"

#define TASK_FREQUENCY_INPUT_MOUSE "task_frequency_input_mouse"
#define TASK_FREQUENCY_INPUT_SENSOR "task_frequency_input_sensor"
#define TASK_FREQUENCY_INPUT_GENERAL "task_frequency_input_general"
#define TASK_FREQUENCY_DISPLAY_REFRESH "task_frequency_display_refresh"

#define MAX_VOLUME "max_volume"
#define MAX_VOLUME_CHANGE_PER_TICK "max_volume_change_per_tick"
#define SAMPLE_RATE "sample_rate"
#define BUFFER_SIZE "buffer_size"

#define WAVEFORM "waveform"
#define LOWEST_NOTE "lowest_note"
#define NUM_OCTAVES "num_octaves"
#define AUTOTUNE_MODE "autotune_mode"

#define TREMOLO_ENABLED "tremolo_enabled"
#define TREMOLO_FREQUENCY "tremolo_frequency"
#define TREMOLO_INTENSITY "tremolo_intensity"

#define UID_FREQUENCY "uid_frequency"
#define UID_VOLUME "uid_volume"
#define HOST "host"
#define PORT "port"

#define SENSOR_FREQ_MIN_VALUE "sensor_freq_min_value"
#define SENSOR_FREQ_MAX_VALUE "sensor_freq_max_value"
#define SENSOR_VOL_MIN_VALUE "sensor_vol_min_value"
#define SENSOR_VOL_MAX_VALUE "sensor_vol_max_value"

#define ACTION_SUSTAIN_NOTE "action_sustain_note"
#define ACTION_OCTAVE_UP "action_octave_up"
#define ACTION_CHANGE_WAVEFORM "action_change_waveform"
#define ACTION_AUTOTUNE_NONE "action_autotune_none"
#define ACTION_AUTOTUNE_SMOOTH "action_autotune_smooth"
#define ACTION_AUTOTUNE_FULL "action_autotune_full"
#define ACTION_TREMOLO "action_tremolo"
#define ACTION_RECORDING_REPLAYING "action_recording_replaying"
#define ACTION_CHORD_MAJOR_1 "action_chord_major_1"
#define ACTION_CHORD_MAJOR_3 "action_chord_major_3"
#define ACTION_CHORD_MAJOR_5 "action_chord_major_5"
#define ACTION_CHORD_MINOR_1 "action_chord_minor_1"
#define ACTION_CHORD_MINOR_3 "action_chord_minor_3"
#define ACTION_CHORD_MINOR_5 "action_chord_minor_5"
#define ACTION_CHORD_CLEAR "action_chord_clear"

/* CONFIGURATION VALUES */

// Input device
#define INPUT_DEVICE_MOUSE "mouse"
#define INPUT_DEVICE_SENSOR "sensor"

// Implemented waveforms
#define WAVE_SIN "sin"
#define WAVE_TRIANGLE "triangle"
#define WAVE_SIN_ASSYM "sin_assym"
#define WAVE_PLATEAU "plateau"
#define WAVE_SIN_INTERFERING "sin_interf"
#define WAVE_SINGLESLIT "singleslit"
#define WAVE_HALFCIRC "halfcirc"
#define WAVE_GAUSS "gauss"
#define WAVE_SAW "saw"
#define WAVE_SQUARE "square"
#define WAVE_COMPLEX "complex"

static const char* WAVE_NAMES[] = {
    WAVE_SIN,
    WAVE_TRIANGLE,
    WAVE_SIN_ASSYM,
    WAVE_PLATEAU,
    WAVE_SIN_INTERFERING,
    WAVE_SINGLESLIT,
    WAVE_HALFCIRC,
    WAVE_GAUSS,
    WAVE_SAW,
    WAVE_SQUARE,
    WAVE_COMPLEX
};

// note frequencies
#define  C_0    130.8127826502992
#define  CIS_0  138.59131548843592
#define  D_0    146.83238395870364
#define  DIS_0  155.56349186104032
#define  E_0    164.81377845643482
#define  F_0    174.6141157165018
#define  FIS_0  184.99721135581706
#define  G_0    195.9977179908745
#define  GIS_0  207.65234878997242
#define  A_0    219.99999999999986
#define  AIS_0  233.08188075904482
#define  B_0    246.94165062806192
#define  C_1    261.6255653005985
#define  CIS_1  277.18263097687196
#define  D_1    293.66476791740746
#define  DIS_1  311.1269837220808
#define  E_1    329.62755691286986
#define  F_1    349.2282314330038
#define  FIS_1  369.99442271163434
#define  G_1    391.99543598174927
#define  GIS_1  415.3046975799451
#define  A_1    440.0
#define  AIS_1  466.1637615180899
#define  B_1    493.8833012561241
#define  C_2    523.2511306011974
#define  CIS_2  554.3652619537443
#define  D_2    587.3295358348153
#define  DIS_2  622.253967444162
#define  E_2    659.2551138257401
#define  F_2    698.456462866008
#define  FIS_2  739.988845423269
#define  G_2    783.9908719634989
#define  GIS_2  830.6093951598906
#define  A_2    880.0000000000003
#define  AIS_2  932.3275230361802
#define  B_2    987.7666025122486
#define  C_3    1046.502261202395
#define  CIS_3  1108.7305239074888
#define  D_3    1174.6590716696307
#define  DIS_3  1244.5079348883241
#define  E_3    1318.5102276514804
#define  F_3    1396.9129257320162
#define  FIS_3  1479.9776908465383
#define  G_3    1567.981743926998
#define  GIS_3  1661.2187903197814
#define  A_3    1760.000000000001
#define  AIS_3  1864.6550460723606
#define  B_3    1975.5332050244976
#define  C_4    2093.0045224047904
#define  CIS_4  2217.4610478149784
#define  D_4    2349.3181433392624
#define  DIS_4  2489.0158697766497
#define  E_4    2637.020455302962
#define  F_4    2793.825851464034
#define  FIS_4  2959.9553816930784
#define  G_4    3135.963487853998
#define  GIS_4  3322.4375806395647
#define  A_4    3520.000000000004
#define  AIS_4  3729.310092144724
#define  B_4    3951.066410048998
#define  C_5    4186.009044809583
#define  CIS_5  4434.92209562996
#define  D_5    4698.6362866785275
#define  DIS_5  4978.031739553302
#define  E_5    5274.040910605927
#define  F_5    5587.651702928071
#define  FIS_5  5919.91076338616
#define  G_5    6271.926975707999
#define  GIS_5  6644.875161279134
#define  A_5    7040.000000000013
#define  AIS_5  7458.6201842894525
#define  B_5    7902.132820098001
#define  C_6    8372.018089619172

static const double NOTES[] = {C_0,CIS_0,D_0,DIS_0,E_0,F_0,FIS_0,G_0,GIS_0,A_0,AIS_0,B_0,C_1,CIS_1,D_1,DIS_1,E_1,F_1,FIS_1,G_1,GIS_1,A_1,AIS_1,B_1,C_2,CIS_2,D_2,DIS_2,E_2,F_2,FIS_2,G_2,GIS_2,A_2,AIS_2,B_2,C_3,CIS_3,D_3,DIS_3,E_3,F_3,FIS_3,G_3,GIS_3,A_3,AIS_3,B_3,C_4,CIS_4,D_4,DIS_4,E_4,F_4,FIS_4,G_4,GIS_4,A_4,AIS_4,B_4,C_5,CIS_5,D_5,DIS_5,E_5,F_5,FIS_5,G_5,GIS_5,A_5,AIS_5,B_5,C_6};
static const char* NOTE_NAMES[] = {"c0","c#0","d0","d#0","e0","f0","f#0","g0","g#0","a0","a#0","b0","c1","c#1","d1","d#1","e1","f1","f#1","g1","g#1","a1","a#1","b1","c2","c#2","d2","d#2","e2","f2","f#2","g2","g#2","a2","a#2","b2","c3","c#3","d3","d#3","e3","f3","f#3","g3","g#3","a3","a#3","b3","c4","c#4","d4","d#4","e4","f4","f#4","g4","g#4","a4","a#4","b4","c5","c#5","d5","d#5","e5","f5","f#5","g5","g#5","a5","a#5","b5","c6"};

// Autotune modes
#define AUTOTUNE_NONE "none"
#define AUTOTUNE_SMOOTH "smooth"
#define AUTOTUNE_FULL "full"

#define CHORD_MODE_1 1
#define CHORD_MODE_3 3
#define CHORD_MODE_5 5
#define CHORD_KEY_MAJOR 1
#define CHORD_KEY_MINOR 2

#endif
