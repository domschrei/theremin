#ifndef THEREMIN_WAVESYNTH_H
#define THEREMIN_WAVESYNTH_H

#include <cmath>
#include <string>

#include "const.h"
#include "configuration.hpp"

class WaveSynth {

// attributes

public:
    // Basic audio properties
    double sample_rate;
    double frequency;
    double volume;
    double period;
    
    struct WaveSmoothing {
        // Properties to ensure continuity (and, to a certain degree, 
        // smoothness of waves), despite frequency and volume changes
        bool waveSwitch;
        double lastWaveAddOffset;
        double lastWaveTotalOffset;
        double wavePeriod;
    };

private:
    Configuration* cfg;
    
    WaveSmoothing waveSmoothing;
    WaveSmoothing waveSmoothingSecondary;
    
    int waveformIdx = 0;
    std::string waveform = WAVE_NAMES[waveformIdx];
    
    // Properties for audio synthesis
    double root12Of2 = std::pow(2.0, 1.0/12); // ratio between two half-tones
    double freqA = 443.0; // calibration of standard pitch
    double freqAis = freqA * root12Of2; // next half-tone next to standard pitch
    double ratioInLog = std::log(freqAis) * M_LOG2E 
            - std::log(freqA) * M_LOG2E; // logarithmic ratio between half-tones
    double minFreq;
    int maxVol;
    int numOctaves;
    
    double mutedVolume = 0;
    bool octaveOffset = false;
    double maxVolumeChangePerTick;
    
    bool tremolo;
    double tremoloIntensity;
    double tremoloFrequency;
    bool tremoloExiting = false;
    double tremoloOffset = 0.0;
    
    double secondaryFrequency;
    double secondaryVolumeShare = 0.4;
    
    std::string autotuneMode;
    
    double volumeTarget = volume;
    
// methods
    
public:
    void init(Configuration* cfg);
    
    uint16_t wave(double t);
    
    void align_frequency();
    void update_frequency(float value);
    void update_volume(float value);
    void toggle_mute();
    void set_octave_offset(bool offset);
    void set_tremolo(bool shouldTremolo);
    void switch_waveform();
    void set_secondary_frequency(double secondaryFrequency);
    bool is_secondary_frequency_active();
    void set_autotune_mode(std::string mode);
    
    double get_max_frequency();
    double get_normalized_frequency(double f);
    int get_nearest_lower_note_index();
    bool is_octave_offset();
    bool is_tremolo_enabled();
    std::string get_waveform();
    std::string get_autotune_mode();
    void volume_tick();

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
