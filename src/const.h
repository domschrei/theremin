#ifndef THEREMIN_CONST_H
#define THEREMIN_CONST_H

#include <SDL2/SDL_keycode.h>

// // ****************************************
// // CONSTANTS
// // ****************************************

// Input device
#define INPUT_DEVICE_MOUSE 1
#define INPUT_DEVICE_SENSOR 2

// Implemented waveforms
#define WAVE_SIN 1
#define WAVE_TRIANGLE 2
#define WAVE_SIN_ASSYM 3
#define WAVE_PLATEAU 4
#define WAVE_SIN_INTERFERING 5
#define WAVE_SINGLESLIT 6
#define WAVE_HALFCIRC 7
#define WAVE_GAUSS 8
#define WAVE_SAW 9
#define WAVE_SQUARE 10

static const char* WAVE_NAMES[] = {"", "sin", "triangle", "sinassym", "plateau", "sininterf", "singleslit", "halfcirc", "gauss", "saw", "square"};

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

#define NUM_NOTES (12 * 6 + 1)

// Autotune modes
#define AUTOTUNE_NONE 1
#define AUTOTUNE_SMOOTH 2
#define AUTOTUNE_FULL 3

// Triggerable actions
#define ACTION_SUSTAINED_NOTE 1
#define ACTION_OCTAVE_UP 2
#define ACTION_CHANGE_WAVEFORM 3
#define ACTION_AUTOTUNE_NONE 4
#define ACTION_AUTOTUNE_SMOOTH 5
#define ACTION_AUTOTUNE_FULL 6
#define ACTION_TREMOLO 7

#endif
