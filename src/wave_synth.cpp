#include <stdlib.h>
#include <numeric>
#include <iostream>
#include <fstream>

#include "wave_synth.h"

/*
 * Generates a single sample (i.e. data point) as a function of t
 * corresponding to the current frequency, volume and waveform.
 */
uint16_t WaveSynth::wave(double t)
{
    
    set_wave_offset(t, &waveSmoothing);
    t += waveSmoothing.lastWaveAddOffset * waveSmoothing.wavePeriod;
    
    double t2 = 0.0;
    double period2;
    if (secondaryFrequency != 0.0) {
        set_wave_offset(t, &waveSmoothingSecondary);
        period2 = waveSmoothingSecondary.wavePeriod;
        t2 = t + waveSmoothingSecondary.lastWaveAddOffset * period2;
    }
    
    auto function = sin;
    
    if (waveform == WAVE_SIN) {
        function = sin;
    } else if (waveform == WAVE_SQUARE) {
        function = square;
    } else if (waveform == WAVE_TRIANGLE) {
        function = triangle;
    } else if (waveform == WAVE_SAW) {
        function = saw;
    } else if (waveform == WAVE_SIN_ASSYM) {
        function = sin_assym;
    } else if (waveform == WAVE_SIN_INTERFERING) {
        function = sin_interfering;
    } else if (waveform == WAVE_PLATEAU) {
        function = plateau;
    } else if (waveform == WAVE_GAUSS) {
        function = gauss;
    } else if (waveform == WAVE_HALFCIRC) {
        function = halfcirc;
    } else if (waveform == WAVE_SINGLESLIT) {
        function = singleslit;
    } else {
        std::cout << "Error: No valid wavetype specified." << std::endl;
        exit(1);
    }
    
    double value = function(t, period, volume);
    
    if (secondaryFrequency != 0.0) {
        double value2 = function(t2, period2, volume);
        return (uint16_t) std::round((1 - secondaryVolumeShare) * value + secondaryVolumeShare * value2);
    } else {
        return (uint16_t) std::round((1 - secondaryVolumeShare) * value);
    }
}

/*
 * Mutes the volume for all samples to be generated from now on.
 * Unmutes if the volume was already muted.
 */
void WaveSynth::toggle_mute() {
    if (mutedVolume == 0.0f) {
        mutedVolume = volume;
        volume = 0.0f;
    } else {
        volume = mutedVolume;
        mutedVolume = 0.0f;
    }
}

void WaveSynth::set_secondary_frequency(double frequency) {
    secondaryFrequency = frequency;
    waveSmoothingSecondary.wavePeriod = period;
    waveSmoothingSecondary.lastWaveAddOffset = 0;
    waveSmoothingSecondary.lastWaveTotalOffset = 0;
    waveSmoothingSecondary.waveSwitch = false;
}

bool WaveSynth::is_secondary_frequency_active() {
    return secondaryFrequency != 0.0;
}

/*
 * Makes all samples (being generated from now on) to be played
 * exactly one octave higher.
 * Also reverts this effect is it is already active.
 */
void WaveSynth::set_octave_offset(bool offset) {
    
    if (offset) {
        octaveOffset = true;
        //frequency *= 2;
    } else {
        octaveOffset = false;
        //frequency /= 2;
    }
    update_frequency(-1);
}

/*
 * Switches to the next waveform, according to the definitions
 * inside config.h.
 */
void WaveSynth::switch_waveform() {
    
    waveform = (waveform % 10) + 1;
    
    // a change of the wave offset is unneccessary
    // as the wave transition will not be continous anyway
}

/*
 * Pitches the current frequency in a soft manner, i.e.
 * a frequency tends to approach the next proper halfnote
 * (but all frequencies inbetween are possible as well).
 */
void WaveSynth::align_frequency() {
    
    if (frequency <= 0 || autotuneMode == AUTOTUNE_NONE)
        return;
    
    double freqInLog = std::log(frequency) * M_LOG2E;
    if (freqInLog <= 0)
        return;
    double freqNew;
    double toneQualitySine = std::sin(
                (freqInLog - std::log(freqA) * M_LOG2E) 
                * 2 * M_PI / ratioInLog
            );
    
    if (autotuneMode == AUTOTUNE_SMOOTH) {
            
        freqNew = frequency - (1 / (ratioInLog * (2 * M_PI))) 
            * toneQualitySine;
            
    } else if (autotuneMode == AUTOTUNE_FULL) {
        
        double lowerFreq = NOTES[get_nearest_lower_note_index()];
        double lowerFreqNorm = get_normalized_frequency(lowerFreq);
        double upperFreq = lowerFreq * root12Of2;
        double upperFreqNorm = get_normalized_frequency(upperFreq);
        double currentFreqNorm = get_normalized_frequency(frequency);
        
        if (upperFreqNorm - currentFreqNorm > currentFreqNorm - lowerFreqNorm) {
            // pitch to lower note
            freqNew = lowerFreq;
        } else {
            // pitch to upper note
            freqNew = upperFreq;
        }
    }
    
    frequency = freqNew;
}

/*
 * Gets a value [0,1] and maps it to a frequency.
 */
void WaveSynth::update_frequency(float value) {
    
    double oldFrequency = frequency;

    if (value != -1) {
        if (octaveOffset) {
            value += 1.0 / NUM_OCTAVES;
        }
        
        frequency = LOWEST_NOTE * std::pow(2, NUM_OCTAVES * value);
    }
    
    if (value == -1 || oldFrequency != frequency) {
        align_frequency();
        waveSmoothing.waveSwitch = true;
        waveSmoothingSecondary.waveSwitch = true;
    }
    period = sample_rate / frequency;
    waveSmoothing.wavePeriod = period;
    
    if (LOG_FREQ) {
        std::cout << frequency << std::endl;
    }
}

/*
 * Gets a value [0,1] and maps it to a volume.
 *
 * However, this does not change the volume directly
 * but just remembers the wished target value for the volume.
 * Periodic calls to volume_tick() will then make the
 * actual volume approach the target value.
 */
void WaveSynth::update_volume(float value) {
    volumeTarget = MAX_VOLUME * value;
}

/*
 * Lets the volume approach the current target volume
 * by a maximal difference of MAX_VOLUME_CHANGE_PER_TICK.
 */
void WaveSynth::volume_tick() {
    double volumeDiff;
    if (volumeTarget - volume > 0) {
        volumeDiff = std::min(MAX_VOLUME_CHANGE_PER_TICK, volumeTarget - volume);
    } else {
        volumeDiff = std::max(-MAX_VOLUME_CHANGE_PER_TICK, volumeTarget - volume);
    }
    volume += volumeDiff;
}

/*
 * Adjusts the horizontal offset of the current wave to make it 
 * continuous (and, to a certain degree, smooth) relative to the
 * wave which was active before.
 */
void WaveSynth::set_wave_offset(double t, WaveSmoothing* smoothing) {
    
    double period = smoothing->wavePeriod;
    double currentBasicOffset = fmod(t, period) / period;
    double currentAddOffset = 0;
    if (smoothing->waveSwitch) {
        currentBasicOffset = fmod(t-1, period) / period;
        currentAddOffset = smoothing->lastWaveTotalOffset - currentBasicOffset;
        smoothing->waveSwitch = false;
    } else {
        currentAddOffset = smoothing->lastWaveAddOffset;
    }
    smoothing->lastWaveTotalOffset = currentBasicOffset + currentAddOffset;
    smoothing->lastWaveAddOffset = currentAddOffset;    
}

void WaveSynth::set_autotune_mode(int mode) {
    autotuneMode = mode;
}

double WaveSynth::get_max_frequency() {
    return LOWEST_NOTE * std::pow(2, NUM_OCTAVES + 1);
}

double WaveSynth::get_normalized_frequency(double f) {
    if (f <= 0)
        return 0.0;
    else 
        return (1.0 / (NUM_OCTAVES + 1)) * std::log2(f / LOWEST_NOTE);
}

bool WaveSynth::is_octave_offset() {
    return octaveOffset;
}

int WaveSynth::get_waveform() {
    return waveform;
}

int WaveSynth::get_autotune_mode() {
    return autotuneMode;
}

/*
 * Does a binary search to find the note (as defined in config.h)
 * which is lower or equals the current frequency, and which has 
 * the smallest distance frequency-wise.
 * Returns the index of this note (a valid index for NOTES and for
 * NOTE_NAMES).
 */
int WaveSynth::get_nearest_lower_note_index() {
    
    int lower = 0;
    int upper = NUM_NOTES - 1;
    
    while (upper - lower > 1) {
        
        int mid = 0.5 * (lower + upper);
        
        if (frequency > NOTES[mid]) {
            lower = std::max(mid, lower+1);
        } else if (frequency < NOTES[mid]) {
            upper = std::min(mid, upper-1);
        } else {
            lower = mid; upper = mid;
        }
    }
    
    return lower;
}

/*
 * STATIC, STATELESS WAVE FUNCTIONS
 * (return value is only a function of the given parameters)
 */

double WaveSynth::sin(double t, double period, double volume) {
    return (volume * (0.5 + 0.5 * std::sin((t) * 2*M_PI / period)));
}

double WaveSynth::sin_assym(double t, double period, double volume) {
    
    double t_mod = fmod(t, period);
    double y = sin_assym_normalized(t_mod / period);
    return (volume * y);
}

double WaveSynth::sin_assym_normalized(double t) {
    
    double y;
    if (t < 0.14) {
        y = std::cos(18 * t - 2) * 0.25 + 0.55;
    } else {
        y = 1.2*(t - 0.73)*(t - 0.73) + 0.35;
    }
    return y;
}

double WaveSynth::sin_interfering(double t, double period, double volume) {
 
    return volume * (0.5 + 0.5 * std::sin((t     ) * 2*M_PI / period)) 
                  * (0.5 + 0.5 * std::sin((4 * t ) * 2*M_PI / period));
}

double WaveSynth::square(double t, double period, double volume) {
    
    double value;
    if (fmod(t, period) <= 0.5 * period) {
        value = 0;
    } else {
        value = 1.0;
    }
    return (volume * value);
}

double WaveSynth::plateau(double t, double period, double volume) {
    
    double value;
    if (fmod(t, period) <= period / 3.0) {
        value = (fmod(t, period) / (period / 3.0));
    } else if (fmod(t, period) <= 2 * period / 3.0) {
        value = 1.0;
    } else {
        value = (-1*(fmod(t, period) - period) / (period / 3.0));
    }
    return (volume * value);
}

double WaveSynth::triangle(double t, double period, double volume) {
    
    double value;
    if (fmod(t, period) <= 0.5 * period) {
        value = 0 + (fmod(t, period) / (0.5 * period));
    } else {
        value = 1 - ((fmod(t, period) - 0.5 * period) / (0.5 * period));
    }
    return (volume * value);
}

double WaveSynth::saw(double t, double period, double volume) {
    
    return (volume * (fmod(t, period) / period));
}

double WaveSynth::gauss(double t, double period, double volume) {
    
    double interval = 40;
    double x = interval * fmod(t, period) / period - (interval / 2);
    double value = std::exp(-(x*x));
    return (volume * value);
}

double WaveSynth::halfcirc(double t, double period, double volume) {
    
    double x = 2 * fmod(t, period) / period - 1;
    double value = std::sqrt(1 - x*x);
    return (volume * value);
} 

double WaveSynth::singleslit(double t, double period, double volume) {
    
    double interval = 20;
    double x = interval * fmod(t, period) / period - (interval / 2);
    double value = std::sin(x) * std::sin(x) / (x * x);
    return (volume * value);
}
