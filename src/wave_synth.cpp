#include <stdlib.h>
#include <numeric>
#include <iostream>
#include <fstream>

#include "wave_synth.hpp"
#include "music_util.hpp"

static WaveSynth::WaveLookupTable complexWaveLookup;

void WaveSynth::init(Configuration* cfg) {
    
    this->cfg = cfg;
    
    sample_rate = cfg->i(SAMPLE_RATE);
    volume = cfg->i(MAX_VOLUME);
    waveform = cfg->str(WAVEFORM);
    period = sample_rate / frequency;
    
    // Find the correct index of the default waveform
    for (int i = 0; i < sizeof(WAVE_NAMES)/sizeof(*WAVE_NAMES); i++) {
        if (WAVE_NAMES[i] == waveform) {
            waveformIdx = i;
            break;
        }
    }
    
    std::string lowest_note = cfg->str(LOWEST_NOTE);
    for (int i = 0; i < sizeof(NOTE_NAMES)/sizeof(*NOTE_NAMES); i++) {
        if (NOTE_NAMES[i] == lowest_note) {
            frequency = NOTES[i];
            minFreq = frequency;
            break;
        }
    }
    numOctaves = cfg->d(NUM_OCTAVES);
    maxVol = cfg->i(MAX_VOLUME);
    maxVolumeChangePerTick = cfg->d(MAX_VOLUME_CHANGE_PER_TICK);
    
    autotuneMode = cfg->str(AUTOTUNE_MODE);
    
    waveSmoothing.waveSwitch = false;
    waveSmoothing.lastWaveAddOffset = 0.0;
    waveSmoothing.lastWaveTotalOffset = 0.0;
    waveSmoothing.wavePeriod = period;
    
    tremolo = cfg->b(TREMOLO_ENABLED);
    tremoloFrequency = cfg->d(TREMOLO_FREQUENCY);
    tremoloIntensity = cfg->d(TREMOLO_INTENSITY);
    
    std::vector<double> frequency_ratios;
    std::vector<double> shares;
    
    frequency_ratios.push_back(1.0);
    frequency_ratios.push_back(2.0);
    frequency_ratios.push_back(3.0);
    frequency_ratios.push_back(4.0);
    frequency_ratios.push_back(5.0);
    frequency_ratios.push_back(6.0);
    frequency_ratios.push_back(7.0);
    frequency_ratios.push_back(8.0);
    frequency_ratios.push_back(9.0);
    frequency_ratios.push_back(10.0);
    frequency_ratios.push_back(11.0);
    frequency_ratios.push_back(12.0);
    frequency_ratios.push_back(13.0);
    frequency_ratios.push_back(14.0);
    frequency_ratios.push_back(15.0);
    frequency_ratios.push_back(16.0);
    frequency_ratios.push_back(17.0);
    frequency_ratios.push_back(18.0);
    frequency_ratios.push_back(19.0);
    frequency_ratios.push_back(20.0);
    
    shares.push_back(1);
    shares.push_back(9);
    shares.push_back(4);
    shares.push_back(2);
    shares.push_back(0.5);
    shares.push_back(0.1);
    shares.push_back(0.2);
    shares.push_back(0.1);
    shares.push_back(0.05);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    shares.push_back(0.01);
    
    // Normalize shares so that they sum up to 1
    double sumShares = 0.0;
    for (int i = 0; i < shares.size(); i++) {
        sumShares += shares[i];
    }
    for (int i = 0; i < shares.size(); i++) {
        shares[i] /= sumShares;
    }
    
    complexWaveLookup.frequency_ratios = frequency_ratios;
    complexWaveLookup.shares = shares;
}

/*
 * Generates a single sample (i.e. data point) as a function of t
 * corresponding to the current frequency, volume and waveform.
 */
uint16_t WaveSynth::wave(double t) {
    
    // Initial time value to pass to children tones
    double old_t = t;
    
    // Find correct position of primary wave (if enabled)
    if (waveSmoothing.active) {
        set_wave_offset(t, &waveSmoothing);
        t += waveSmoothing.lastWaveAddOffset * waveSmoothing.wavePeriod;
    }
    
    // Find correct position of secondary wave (if enabled)
    double t2 = 0.0;
    double period2;
    if (secondaryFrequency != 0.0) {
        set_wave_offset(t, &waveSmoothingSecondary);
        period2 = waveSmoothingSecondary.wavePeriod;
        t2 = t + waveSmoothingSecondary.lastWaveAddOffset * period2;
    }
    
    // Apply tremolo effect (if enabled)
    double volumeToUse = (tremolo ? get_tremolo_volume(t) : volume);
    
    // Find correct wave function
    auto function = get_wave_function();
    
    // Calculate basic value
    double value = function(t, period, volumeToUse);
    
    // Calculate secondary value (if enabled) and adapt volume
    if (secondaryFrequency != 0.0) {
        double value2 = function(t2, period2, volumeToUse);
        value = std::round((1 - secondaryVolumeShare) * value + secondaryVolumeShare * value2);
    } else {
        value = std::round((1 - secondaryVolumeShare) * value);
    }
    
    // Adapt volume for chords 
    // and call the additional tones' wave function to accumulate the sound
    value /= 3;
    double childVals1 = 0;
    double childVals2 = 0;
    if (children.size() > 0) {
        for (int i = 0; i < children.size(); i++) {
            childVals1 += children[i].wave(old_t);
        }
    }
    if (fading && nextChildren.size() > 0) {
        for (int i = 0; i < nextChildren.size(); i++) {
            childVals2 += nextChildren[i].wave(old_t);
        }
    }
    value += fadingOutFactor * childVals1 + fadingInFactor * childVals2;
    
    return (uint16_t) value;
}

void WaveSynth::add_child_note(int rel_halftones) {
    
    int noteIdx = MusicUtil::get_nearest_note_index(frequency);
    double note = NOTES[noteIdx] * std::pow(root12Of2, rel_halftones);
    
    WaveSynth child_synth;
    child_synth.init(cfg);
    child_synth.frequency = note;
    child_synth.period = sample_rate / child_synth.frequency;
    child_synth.waveSmoothing.active = false;
    child_synth.update_volume(volumeTarget);
    child_synth.set_secondary_frequency(0.0);
    child_synth.waveformIdx = waveformIdx;
    child_synth.waveform = waveform;
    nextChildren.push_back(child_synth);
}

void WaveSynth::set_chord_notes(int chordMode, int chordKey) {
   
    nextChildren.clear();
    
    std::vector<int> intervals = MusicUtil::get_chord_intervals(chordMode, chordKey);
    for (int i = 0; i < intervals.size(); i++) {
        add_child_note(intervals[i]);
    }
    
    fadingOutFactor = 1;
    fadingInFactor = 0;
    fading = true;
    
    int chordNoteIdx = MusicUtil::get_nearest_note_index(frequency);
    currentChordName = MusicUtil::get_chord_name(
                chordNoteIdx, chordMode, chordKey);
}

void WaveSynth::clear_child_notes() {
    nextChildren.clear();
    fadingOutFactor = 1;
    fadingInFactor = 0;
    fading = true;
    currentChordName = "";
}

bool WaveSynth::has_child_notes() {
    return children.size() > 0;
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
    
    octaveOffset = offset;
    update_frequency(-1);
}

/*
 * Starts or stops the tremolo effect.
 * Stopping does not take plase immediately but at the next 
 * appropriate sample such that there will be no cracking.
 */
void WaveSynth::set_tremolo(bool shouldTremolo) {
    
    if (shouldTremolo) {
        tremolo = true;
        tremoloOffset = 0.0;
        tremoloExiting = false;
    } else {
        tremoloExiting = true;
    }
}

/*
 * Switches to the next waveform, according to the definitions
 * inside config.h.
 */
void WaveSynth::switch_waveform() {
    
    waveformIdx = (waveformIdx + 1) % (sizeof(WAVE_NAMES)/sizeof(*WAVE_NAMES));
    waveform = WAVE_NAMES[waveformIdx];
    
    // a change of the wave offset is unneccessary
    // as the wave transition will not be continous anyway
}

/*
 * Pitches the current frequency in a soft manner, i.e.
 * a frequency tends to approach the next proper halfnote
 * (but all frequencies inbetween are possible as well).
 */
void WaveSynth::align_frequency() {
    
    if (frequency <= 0 || autotuneMode == AUTOTUNE_NONE) {
        return;
    }
    
    double freqNew;
    
    if (autotuneMode == AUTOTUNE_SMOOTH) {
            
        double freqInLog = std::log(frequency) * M_LOG2E;
        if (freqInLog <= 0) {
            return;
        }
        double toneQualitySine = std::sin(
                    (freqInLog - std::log(freqA) * M_LOG2E) 
                    * 2 * M_PI / ratioInLog
                );
        freqNew = frequency - (1 / (ratioInLog * (2 * M_PI))) 
            * toneQualitySine;
            
    } else /*if (autotuneMode == AUTOTUNE_FULL)*/ {
        
        freqNew = NOTES[MusicUtil::get_nearest_note_index(frequency)];
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
            value += 1.0 / numOctaves;
        }
        
        frequency = minFreq * std::pow(2, numOctaves * value);
    }
    
    if (value == -1 || oldFrequency != frequency) {
        align_frequency();
        waveSmoothing.waveSwitch = true;
        waveSmoothingSecondary.waveSwitch = true;
    }
    period = sample_rate / frequency;
    waveSmoothing.wavePeriod = period;
    
    if (cfg->b(LOG_FREQ)) {
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
    volumeTarget = maxVol * value;
    
    // Also update volume for child tones
    for (int i = 0; i < children.size(); i++) {
        children[i].update_volume(value);
    }
    for (int i = 0; i < nextChildren.size(); i++) {
        nextChildren[i].update_volume(value);
    }
}

/*
 * Lets the volume approach the current target volume
 * by a maximal difference of MAX_VOLUME_CHANGE_PER_TICK.
 * Also advances and finalizes crossfading of chords, if currently active.
 */
void WaveSynth::volume_tick() {
    
    // Adjust volume
    double volumeDiff;
    if (volumeTarget - volume > 0) {
        volumeDiff = std::min(maxVolumeChangePerTick, volumeTarget - volume);
    } else {
        volumeDiff = std::max(-maxVolumeChangePerTick, volumeTarget - volume);
    }
    volume += volumeDiff;
    
    // Adjust volume of children tones
    for (int i = 0; i < children.size(); i++) {
        children[i].volume_tick();
    }
    for (int i = 0; i < nextChildren.size(); i++) {
        nextChildren[i].volume_tick();
    }
    
    // Cross-fade between current and incoming child tones
    if (fading) {
        fadingOutFactor -= 0.02;
        fadingInFactor += 0.02;
        
        if (fadingOutFactor <= 0 && fadingInFactor >= 1) {
            children = nextChildren;
            nextChildren.clear();
            fadingOutFactor = 1;
            fadingInFactor = 1;
            fading = false;
        }
    }
}

/*
 * Adjusts the horizontal offset of the current wave to make it 
 * continuous (and, to a certain degree, smooth) relative to the
 * wave which was active before.
 */
void WaveSynth::set_wave_offset(double t, WaveSmoothing* smoothing) {
    
    if (!smoothing->active) 
        return;
    
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

void WaveSynth::set_autotune_mode(std::string mode) {
    autotuneMode = mode;
}

double WaveSynth::get_max_frequency() {
    return minFreq * std::pow(2, numOctaves + 1);
}

bool WaveSynth::is_octave_offset() {
    return octaveOffset;
}

bool WaveSynth::is_tremolo_enabled() {
    return tremolo;
}

std::string WaveSynth::get_waveform() {
    return waveform;
}

std::string WaveSynth::get_autotune_mode() {
    return autotuneMode;
}

std::string WaveSynth::get_current_chord_name() {
    return currentChordName;
}

/*
 * Maps a frequency inside the minimum and maximum frequency bounds
 * to a linear value in [0,1], such that two pairs of tones with the same
 * interval will have the same absolute distance on this scale.
 * (Stateless function)
 */
double WaveSynth::get_normalized_frequency(double f) {
    if (f <= 0) {
        return 0.0;
    } else {
        return (1.0 / (numOctaves + 1)) * std::log2(f / minFreq);
    }
}

/*
 * Determines the volume that should be used for the current tick 
 * if the tremolo effect is enabled. The actual volume is being modulated
 * by adding a sine wave. Also finalizes the effect if it is to fade out.
 */
double WaveSynth::get_tremolo_volume(double t) {
    
    double volumeToUse;
    
    if (tremoloOffset == 0.0) {
        tremoloOffset = t;
    }
    // Add a sine wave to the volume in order to modulate it
    volumeToUse = volume + tremoloIntensity * volume 
        * std::sin(t * 2 * M_PI / (sample_rate / tremoloFrequency)
        - tremoloOffset * 2 * M_PI / (sample_rate / tremoloFrequency));
    volumeToUse = std::min(volumeToUse, (double) maxVol);
    volumeToUse = std::max(volumeToUse, 0.0);
    
    // If tremolo is to fade out, 
    // check if now is an appropriate time to smoothly stop the effect
    if (tremoloExiting && std::abs(volumeToUse - volume) <= maxVol / 1000.0) {
        tremolo = false;
    }
    
    return volumeToUse;
}

/*
 * Returns the wave function which should be used according to the object's
 * waveform attribute. Exits the program with an error message if no waveform
 * is specified.
 */
wavefunc WaveSynth::get_wave_function() {
    
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
    } else if (waveform == WAVE_COMPLEX) {
        function = complex;
    } else {
        std::cout << "Error: No valid wavetype specified." << std::endl;
        exit(1);
    }
    
    return function;
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
 
    return volume * (0.5 + 0.5 * std::sin((t    ) * 2*M_PI / period)) 
                  * (0.5 + 0.5 * std::sin((4 * t) * 2*M_PI / period));
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
    double sin = std::sin(x);
    double value = sin * sin / (x * x);
    return (volume * value);
}

double WaveSynth::complex(double t, double period, double volume) {
        
    double sum = 0;
    int numWaves = std::min(complexWaveLookup.frequency_ratios.size(), complexWaveLookup.shares.size());
    for (int i = 0; i < numWaves; i++) {
        sum += WaveSynth::sin(t, period / complexWaveLookup.frequency_ratios[i], volume * complexWaveLookup.shares[i]);
    }
    return sum;
}
