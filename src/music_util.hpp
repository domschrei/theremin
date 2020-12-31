#ifndef THEREMIN_MUSIC_UTIL_H
#define THEREMIN_MUSIC_UTIL_H

#include <vector>
#include <string>
#include <algorithm>

class MusicUtil {

public:
    static int get_nearest_lower_note_index(double frequency);
    static int get_nearest_note_index(double frequency);
    
    static std::vector<int> get_chord_intervals(int chordMode, int chordKey);
    static std::string get_chord_name(int noteNameIdx,int chordMode, int chordKey);
    
    struct frequency_correction {
        bool lower_note_nearer;
        float rel_error;
    };
    static frequency_correction get_error_of_frequency(double norm_freq, double lower_norm_freq, double upper_norm_freq);
};

#endif
