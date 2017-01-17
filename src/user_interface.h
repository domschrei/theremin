#ifndef THEREMIN_MOUSE_H
#define THEREMIN_MOUSE_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "wave_synth.h"

class UserInterface {
  
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
    const char* HELP_TEXT_3 = "Press {a,b,c} to trigger";
    const char* HELP_TEXT_4 = "the corresponding pedals.";
    const char* HELP_TEXT_5 = "Press {1,2,3} to set an";
    const char* HELP_TEXT_6 = "autotune mode.";
    
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
    void clean_up();
};

#endif
