#include <iostream>
#include <cmath>
#include <fstream>
#include <numeric>
#include <functional>
#include <string> 
#include <vector>

#include "config.h"
#include "audio.h"

/* 
 * Singleton-like object (because a static callback function is needed)
 */
Audio stdAudio;
Audio* Audio::get_std_audio() {
    return &stdAudio;
}

/* 
 * Simple Mutex implementation for audio buffer manipulation
 */
void wait(Audio* a) {
    // wait for critical section to become accessible    
    a->mutex.lock();
}
void notify(Audio* a) {
    // leave critical section
    a->mutex.unlock();
}

/* 
 * The audio function callback 
 */
void fill_audio(void *udata, Uint8 *stream, int len) {

    Audio *a = Audio::get_std_audio();

    if (a->exiting) {
        return;
    }
    
    wait(a);
    
    // Sanity checks
    if (len != AUDIO_BUFFER_SIZE / 2) {
        fprintf(stderr, "Audio: Invalid read length of %i; should be %i.\n"
        "Please make sure that the buffer size (AUDIO_BUFFER_SIZE)\n"
        "is compliant with the advertised read size (AUDIO_BUFFER_ADVERTISED_READ_SIZE).\n", 
        len, AUDIO_BUFFER_SIZE / 2);
        exit(1);
    }
    if ( a->unreadSamples < len ) {
        fprintf(stderr, "Audio Error: Not enough samples left to read.\n"
            "Maybe the computer is overburdened and you should stop\n"
            "other processes, optimize this program's compilation,\n"
            "or disable the graphical real-time screen (if enabled).\n"
        );
        a->isPlaying = false;
        notify(a);
        return;
    }
    
    // Moves start index of buffer reading, if neccessary
    int idx = len;
    if (a->audio_pos >= &(a->buffer[a->bufferSize])) {
        a->audio_pos = &(a->buffer[0]);
        idx = 0;
    }   
    
    // Mix data
    len = ( len > a->unreadSamples ? a->unreadSamples : len );
    //SDL_MixAudio(stream, a->audio_pos, len, a->volume);
    SDL_memcpy(stream, a->audio_pos, len); 
    
    // Update data structure for next read / write
    a->audio_pos += len;
    a->unreadSamples -= len;
    
    notify(a);
}

/*
 * Volume setter, with the new volume given as a float 
 * between 0 (muted) and 1 (max).
 */
void Audio::set_volume(float volume_0_to_1) {
    
    volume = std::round(SDL_MIX_MAXVOLUME * volume_0_to_1);
}

/*
 * Initial audio settings
 */
void Audio::setup_audio() {
    
    bufferIdx = 0;
    bufferSize = AUDIO_BUFFER_SIZE;

    /* Initialize defaults, Video and Audio 
    if((SDL_Init(SDL_INIT_AUDIO) == -1)) { 
        printf("Could not initialize SDL audio: %s.\n", SDL_GetError());
        exit(-1);
    }*/
    
    SDL_AudioSpec wanted;

    /* Set the audio format */
    wanted.freq = AUDIO_SAMPLE_RATE;
    wanted.format = AUDIO_U8;
    wanted.channels = 1;    /* 1 = mono, 2 = stereo */
    wanted.samples = AUDIO_BUFFER_ADVERTISED_READ_SIZE;
    wanted.callback = fill_audio;
    wanted.userdata = NULL;

    /* Open the audio device, forcing the desired format */
    if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
        fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
        exit(1);
    }
}

/*
 * Should be called *once* when the audio buffer is full
 * and when it will be refilled frequently.
 */
void Audio::start_playing() {
    
    wait(this);
    
    audio_pos = &buffer[0];
    /* Let the callback function play the audio */
    SDL_PauseAudio(0);
    std::cerr << "Started playing." << std::endl;
    isPlaying = true;
    
    notify(this);
}

/*
 * Adds a single sample to the current audio buffer.
 */
void Audio::new_sample(uint8_t sample) {
    
    wait(this);
    
    if (bufferIdx == bufferSize) {
        bufferIdx = 0;
    }
    
    buffer[bufferIdx] = sample;
    
    if (LOG_DATA) {
        std::cout << " " << (int) buffer[bufferIdx] << std::endl;
    }
    bufferIdx++;
    unreadSamples++;
    
    notify(this);
}

/*
 * Checks whether the audio buffer is full.
 */
bool Audio::is_buffer_full() {
    return unreadSamples == AUDIO_BUFFER_SIZE;
}

/*
 * Pauses playing, when should_pause is true.
 * Continues playing, else.
 */
void Audio::pause(bool should_pause) {
    
    int pauseInt = should_pause ? 1 : 0;
    
    wait(this);
    
    SDL_PauseAudio(pauseInt);
    
    notify(this);
}

/*
 * Completely clears the audio buffer.
 */
void Audio::reset() {
    
    wait(this);
    
    bufferIdx = 0;
    unreadSamples = 0;
    memset(&buffer[0], 0, sizeof(buffer));
    
    notify(this);
}
