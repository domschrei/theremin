#ifndef THEREMIN_AUDIO_H
#define THEREMIN_AUDIO_H

#include <stdint.h>
#include <mutex>
#include <SDL2/SDL.h>

class Audio {

public:
    static Audio* get_std_audio();
    
    void setup_audio();
    void start_playing();
    void new_sample(uint8_t sample);
    bool is_buffer_full();
    void reset();
    void pause(bool should_pause);
    void set_volume(float volume_0_to_1);

    Uint32 unreadSamples;
    
    Uint8 *audio_pos;
    int bufferSize;
    Uint8 buffer[1024];
    int bufferIdx;
    
    int volume = SDL_MIX_MAXVOLUME;
    
    // for critical section with audio buffer manipulation
    std::mutex mutex;
    
    bool isPlaying = false;
    bool exiting = false;
};
    
#endif
