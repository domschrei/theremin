#ifndef THEREMIN_MOUSE_H
#define THEREMIN_MOUSE_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "wave_synth.h"

class UserInterface {
    
public:
    void setup();
    bool* poll_events();
    void fetch_input(float *x, float *y);
    void refresh_surface(WaveSynth* synth);
    void clean_up();
  
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
    
    const char* HELP_TEXT_1 = "Use the sensors to control";
    const char* HELP_TEXT_2 = "frequency and volume.";
    const char* HELP_TEXT_3 = "Press the corresponding keys";
    const char* HELP_TEXT_4 = "to trigger the effects.";
};

#endif
