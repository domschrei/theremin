#ifndef THEREMIN_MOUSE_H
#define THEREMIN_MOUSE_H

#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

#include "const.h"
#include "configuration.h"
#include "wave_synth.h"

class UserInterface {
    
public:
    void setup(Configuration* cfg, WaveSynth* synth);
    std::vector<std::string> poll_events();
    void last_cursor_position(float *x, float *y);
    void refresh_surface();
    void clean_up();
  
private:
    Configuration* cfg;
    WaveSynth* synth;
    
    float mouse_x;
    float mouse_y;
    
    int window_w = 600;
    int window_h = 450;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    TTF_Font* sans;
    TTF_Font* sansLarge;
    
    std::string lastWaveform;
    std::string lastAutotuneMode;
    
    const char* HELP_TEXT_1 = "Use the sensors to control";
    const char* HELP_TEXT_2 = "frequency and volume.";
    const char* HELP_TEXT_3 = "Press the corresponding keys";
    const char* HELP_TEXT_4 = "to trigger the effects.";
    
    
    /* Drawing routines, from high to low abstraction */

    void draw_effect_labels();
    void draw_note_display();
    void draw_status_bars();
    void draw_help_text();
    
    void draw_pedal(const char* text, int x, int y, bool isPressed);
    void draw_progress(SDL_Rect rect, float share, bool inReadDirection);
    void draw_text(const char* text, int x, int y, SDL_Color color, TTF_Font* font);
    
    void round_corners(SDL_Rect rect);
};

#endif
