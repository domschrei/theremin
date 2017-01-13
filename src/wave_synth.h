#ifndef THEREMIN_WAVESYNTH_H
#define THEREMIN_WAVESYNTH_H

#include <cmath>

#include "config.h"

class WaveSynth {

// attributes

public:
    // Basic audio properties
    double sample_rate = 16384;
    double frequency = LOWEST_NOTE; // will be changed by 1st input
    double volume = 255;
    double period = sample_rate / frequency; // will be changed by 1st input
    
    struct WaveSmoothing {
        // Properties to ensure continuity (and, to a certain degree, 
        // smoothness of waves), despite frequency and volume changes
        bool waveSwitch = false;
        double lastWaveAddOffset = 0.0;
        double lastWaveTotalOffset = 0.0;
        double wavePeriod = 16384 / LOWEST_NOTE;
    };

private:
    
    WaveSmoothing waveSmoothing;
    WaveSmoothing waveSmoothingSecondary;
    int waveform = WAVEFORM + 0;
    
    // Properties for audio synthesis
    double root12Of2 = std::pow(2.0, 1.0/12); // ratio between two half-tones
    double freqA = 443.0; // calibration of standard pitch
    double freqAis = freqA * root12Of2; // next half-tone next to standard pitch
    double ratioInLog = std::log(freqAis) * M_LOG2E 
            - std::log(freqA) * M_LOG2E; // logarithmic ratio between half-tones
    //double lastFrequency = frequency;

    double mutedVolume = 0;
    bool octaveOffset = false;
    bool vibrate = false;
    
    double secondaryFrequency;
    double secondaryVolumeShare = 0.4;
    
// methods
    
public:
    uint8_t wave(double t);
    
    void align_frequency();
    void update_frequency(float value);
    void update_volume(float value);
    void toggle_mute();
    void set_octave_offset(bool offset);
    void set_vibrate(bool shouldVibrate);
    void switch_waveform();
    void set_secondary_frequency(double secondaryFrequency);
    bool is_secondary_frequency_active();
    
    double get_max_frequency();
    double get_normalized_frequency(double f);
    int get_nearest_lower_note_index();
    bool is_octave_offset();
    int get_waveform();

private:
    void set_wave_offset(double t, WaveSmoothing* smoothing);
    
    static double sin(double t, double period, double volume);
    static double sin_assym(double t, double period, double volume);
    static double sin_assym_normalized(double t);
    static double sin_interfering(double t, double period, double volume);
    static double square(double t, double period, double volume);
    static double plateau(double t, double period, double volume);
    static double triangle(double t, double period, double volume);
    static double saw(double t, double period, double volume);
    static double gauss(double t, double period, double volume);
    static double halfcirc(double t, double period, double volume);
    static double singleslit(double t, double period, double volume);
};

#endif
