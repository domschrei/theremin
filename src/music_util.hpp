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
};

#endif
