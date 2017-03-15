#ifndef THEREMIN_AUDIO_H
#define THEREMIN_AUDIO_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include <vector>

#include "configuration.hpp"

class Audio {

public:    
    void setup_audio(Configuration* cfg);
    void start_playing();
    void start_recording();
    void stop_recording();
    void start_replaying();
    void stop_replaying();
    bool new_sample(uint16_t sample);
    void reset();
    void set_volume(float volume_0_to_1);
    bool is_buffer_full();
    void set_exiting(bool isExiting);
    bool is_playing();
    bool is_recording();
    bool is_replaying();

private:
    bool flush_buffer_to_sdl();
    
    int deviceId;
    
    bool exiting = false;
    bool isPlaying = false;
    bool isRecording = false;
    bool isReplaying = false;
    
    Configuration* cfg;
    
    int bufferSize;
    Uint16* buffer;
    int bufferIdx;
    
    std::vector<Uint16> recordingBuffer;
    int recordingBufferIdx;
};
    
#endif
