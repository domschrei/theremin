#include <iostream>

#include "const.h"
#include "audio.h"

/*
 * Convert between some amount of samples
 * and the corresponding amount of bytes.
 * 1 Sample = 16 bit = 2 byte
 */
int bytes_to_samples(int bytes) {
    return bytes / 2;
}
int samples_to_bytes(int samples) {
    return samples * 2;
}

/*
 * Initial audio settings
 */
void Audio::setup_audio(Configuration* cfg) {
    
    bufferIdx = 0;
    this->cfg = cfg;
    bufferSize = cfg->i("buffer_size");
    buffer = new Uint16[bufferSize];
    
    SDL_AudioSpec wanted, having;

    /* Set the audio format */
    wanted.freq = cfg->i("sample_rate");
    wanted.format = AUDIO_U16;
    wanted.channels = 1;    /* 1 = mono, 2 = stereo */
    wanted.samples = bufferSize;
    wanted.callback = NULL;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    deviceId = SDL_OpenAudioDevice(NULL, 0, &wanted, &having, 0);
    if (deviceId < 0) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(1);
    }
}

/*
 * Should be called *once* when the audio buffer is full
 * and when it will be refilled frequently.
 */
void Audio::start_playing() {
    
    std::cout << "Started playing." << std::endl;
    isPlaying = true;
    
    flush_buffer_to_sdl();
}

/*
 * Adds a single sample to the current audio buffer.
 * Also flushes the audio buffer to SDL when it is full.
 */
bool Audio::new_sample(uint16_t sample) {
    
    if (exiting)
        return false;
            
    bool played;
    
    if (bufferIdx < bufferSize) {
        buffer[bufferIdx] = sample;
        
        if (cfg->b("log_data")) {
            std::cout << " " << (int) buffer[bufferIdx] << std::endl;
        }
        bufferIdx++;
        played = true;
    }
    
    if (bufferIdx == bufferSize) {
        played = flush_buffer_to_sdl();
    }
    
    return played;
}

/*
 * If the buffer is full and if the SDL-internal queue of audio samples
 * is empty, the buffer data will be put into the queue.
 * Afterwards, the buffer can be overwritten with new data.
 * Returns true iff the buffer has been flushed.
 */
bool Audio::flush_buffer_to_sdl() {
    
    if (isPlaying && bytes_to_samples(SDL_GetQueuedAudioSize(deviceId)) == 0) {
        
        int errCode = SDL_QueueAudio(deviceId, &buffer[0], 
                                     samples_to_bytes(bufferSize));
        if (errCode != 0) {
            fprintf(stderr, "Couldn't play audio: %s\n", SDL_GetError());
            exit(1);
        } else {
            
            SDL_PauseAudioDevice(deviceId, 0);
            bufferIdx = 0;
            return true;
        }
        
    }
    return false;
}

/*
 * Checks whether the audio buffer is full.
 */
bool Audio::is_buffer_full() {
    return bufferIdx == bufferSize;
}

/*
 * Completely clears the audio buffer.
 */
void Audio::reset() {
        
    bufferIdx = 0;
    memset(&buffer[0], 0, sizeof(buffer));
}

/*
 * Tell the audio object if it should prepare itself
 * for exiting.
 */
void Audio::set_exiting(bool isExiting) {
    
    this->exiting = isExiting;
    isPlaying = false;
    delete buffer;
}

/*
 * Returns true if the program is playing
 * audio at the moment.
 */
bool Audio::is_playing() {
    return isPlaying;
}
