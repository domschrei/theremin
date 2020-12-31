#include "music_util.hpp"
#include "const.h"
#include <cmath>

std::vector<int> MusicUtil::get_chord_intervals(int chordMode, int chordKey) {
    
    std::vector<int> chordIntervals;
    
    if (chordMode == CHORD_MODE_1 && chordKey == CHORD_KEY_MAJOR) {
        chordIntervals.push_back(-5);
        chordIntervals.push_back(-8);
    } else if (chordMode == CHORD_MODE_3 && chordKey == CHORD_KEY_MAJOR) {
        chordIntervals.push_back(-4);
        chordIntervals.push_back(-9);
    } else if (chordMode == CHORD_MODE_5 && chordKey == CHORD_KEY_MAJOR) {
        chordIntervals.push_back(-3);
        chordIntervals.push_back(-7);
    } else if (chordMode == CHORD_MODE_1 && chordKey == CHORD_KEY_MINOR) {
        chordIntervals.push_back(-5);
        chordIntervals.push_back(-9);
    } else if (chordMode == CHORD_MODE_3 && chordKey == CHORD_KEY_MINOR) {
        chordIntervals.push_back(-3);
        chordIntervals.push_back(-8);
    } else if (chordMode == CHORD_MODE_5 && chordKey == CHORD_KEY_MINOR) {
        chordIntervals.push_back(-4);
        chordIntervals.push_back(-7);
    }
    
    return chordIntervals;
}

std::string MusicUtil::get_chord_name(int noteNameIdx, int chordMode, int chordKey) {
    
    if (noteNameIdx < 12) {
        noteNameIdx += 12;
    }
    if (noteNameIdx + 12 >= sizeof(NOTE_NAMES)/sizeof(NOTE_NAMES[0])) {
        noteNameIdx -= 12;
    }
    
    std::string name;
    if (chordMode == CHORD_MODE_1) {
        name = NOTE_NAMES[noteNameIdx];
    } else if (chordMode == CHORD_MODE_3) {
        if (chordKey == CHORD_KEY_MAJOR) {
            name = NOTE_NAMES[noteNameIdx - 4];
        } else {
            name = NOTE_NAMES[noteNameIdx - 3];
        }
    } else /* if (chordMode == CHORD_MODE_5) */ {
        name = NOTE_NAMES[noteNameIdx - 7];
    }
    if (chordKey == CHORD_KEY_MAJOR) {
        std::transform(name.begin(), name.end(), name.begin(), ::toupper);
    }
    name = name.substr(0, name.length() - 1);
    return name;
}

/*
 * Finds the note (as defined in config.h)
 * which is lower or equals the current frequency, and which has 
 * the smallest distance frequency-wise.
 * Returns the index of this note (a valid index for NOTES and for
 * NOTE_NAMES).
 */
int MusicUtil::get_nearest_lower_note_index(double frequency) {
    
    int lower = 0;
    int upper = sizeof(NOTE_NAMES)/sizeof(*NOTE_NAMES) - 1;
    
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
 * Finds the note which ressembles the current frequency most closely.
 * The corresponding frequency is obtained via NOTES[return_value].
 */
int MusicUtil::get_nearest_note_index(double frequency) {
    
    int lowerNoteIdx = get_nearest_lower_note_index(frequency);
    double lowerNote = NOTES[lowerNoteIdx];
    double upperNote = NOTES[lowerNoteIdx + 1];
    double lowerNoteNorm = 0;
    double upperNoteNorm = std::log2(upperNote / lowerNote);
    double currNoteNorm = std::log2(frequency / lowerNote);
    
    int noteIdx;
    if (upperNoteNorm - currNoteNorm > currNoteNorm - lowerNoteNorm) {
        noteIdx = lowerNoteIdx;
    } else {
        noteIdx = lowerNoteIdx + 1;
    }
    
    return noteIdx;
}

MusicUtil::frequency_correction MusicUtil::get_error_of_frequency(double norm_freq, double lower_norm_freq, double upper_norm_freq) {
    
    frequency_correction correction;
    double freqDiffNorm = upper_norm_freq - lower_norm_freq;
    float diffUpper = upper_norm_freq - norm_freq;
    float diffLower = norm_freq - lower_norm_freq;
    if (diffLower <= diffUpper) {
        correction.lower_note_nearer = true;
        correction.rel_error = diffLower / (0.5 * freqDiffNorm);
    } else {
        correction.lower_note_nearer = false;
        correction.rel_error = diffUpper / (0.5 * freqDiffNorm);
    }
    return correction;
}
