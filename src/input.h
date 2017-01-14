#ifndef THEREMIN_MOUSE_H
#define THEREMIN_MOUSE_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "wave_synth.h"

class Input {
  
private:
    float mouse_x;
    float mouse_y;
    
    int window_w = 600;
    int window_h = 450;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    TTF_Font* sans;
    TTF_Font* sansLarge;
    
    void draw_pedal(const char* text, int x, int y, bool isPressed);
    void draw_progress(SDL_Rect rect, float share, bool inReadDirection);
    void draw_text(const char* text, int x, int y, SDL_Color color, TTF_Font* font);
    void round_corners(SDL_Rect rect);
    
    int lastWaveform = WAVEFORM;
    int lastAutotuneMode = AUTOTUNE_MODE;
    
public:
    const static int INPUT_PRESS_A = 1;
    const static int INPUT_PRESS_B = 2;
    const static int INPUT_PRESS_C = 3;
    const static int INPUT_RELEASE_A = 4;
    const static int INPUT_RELEASE_B = 5;
    const static int INPUT_RELEASE_C = 6;
    const static int INPUT_PRESS_1 = 7;
    const static int INPUT_PRESS_2 = 8;
    const static int INPUT_PRESS_3 = 9;
    const static int INPUT_RELEASE_1 = 10;
    const static int INPUT_RELEASE_2 = 11;
    const static int INPUT_RELEASE_3 = 12;
    
    void setup();
    bool* poll_events();
    void fetch_input(float *x, float *y);
    void refresh_surface(WaveSynth* synth);
};

#endif
