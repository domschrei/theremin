#ifndef THEREMIN_AUDIO_H
#define THEREMIN_AUDIO_H

#include <stdint.h>
#include <SDL2/SDL.h>

class Audio {

public:    
    void setup_audio();
    void start_playing();
    bool new_sample(uint16_t sample);
    void reset();
    void set_volume(float volume_0_to_1);
    bool is_buffer_full();
    void set_exiting(bool isExiting);
    bool is_playing();

private:
    bool flush_buffer_to_sdl();
    
    int deviceId;
    
    bool exiting = false;
    bool isPlaying = false;
    
    int bufferSize;
    Uint16 buffer[AUDIO_BUFFER_SIZE];
    int bufferIdx;
};
    
#endif
