#ifndef THEREMIN_MOUSE_H
#define THEREMIN_MOUSE_H

#include "const.h"
#include "SDL2/SDL.h"

#ifdef THEREMIN_GUI
#include <SDL2/SDL_ttf.h>
#else
#include <curses.h>
#endif

#include "configuration.hpp"
#include "wave_synth.hpp"
#include "audio.hpp"

class UserInterface {
    
public:
    void setup(Configuration* cfg, WaveSynth* synth, Audio* audio);
    std::vector<std::string> poll_events();
    void last_cursor_position(float *x, float *y);
    void clean_up();
    
    void refresh_surface();

private:
    Configuration* cfg;
    WaveSynth* synth;
    Audio* audio;
    
    const char* HELP_TEXT_1 = "Use the sensors to control";
    const char* HELP_TEXT_2 = "frequency and volume.";
    const char* HELP_TEXT_3 = "Press the corresponding keys";
    const char* HELP_TEXT_4 = "to trigger the effects.";
    
    float mouse_x;
    float mouse_y;
    
    int window_w = 600;
    int window_h = 450;
    
#ifdef THEREMIN_GUI
    
    // GUI drawing stuff
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    
    TTF_Font* sans;
    TTF_Font* sansLarge;
    
    std::string lastWaveform;
    std::string lastAutotuneMode;
    
    /* Drawing routines, from high to low abstraction */

    void draw_effect_labels();
    void draw_note_display();
    void draw_status_bars();
    void draw_help_text();
    void draw_chords();
    
    void draw_pedal(const char* text, int x, int y, bool isPressed);
    void draw_pedal(const char* text, int x, int y, int w, int h, bool isPressed);
    void draw_progress(SDL_Rect rect, float share, bool inReadDirection);
    void draw_text(const char* text, int x, int y, SDL_Color color, TTF_Font* font);
    
    void round_corners(SDL_Rect rect);
    
#else
    
    // CLI printing stuff
    
    void print_box(int width, int height);
    void print_current_volume();
    void print_current_note();
    void print_current_chords();
    void print_help_text();
    void print_buttons();
    
    void print_and_wrap(std::string str);
    void print_linebreak();
    
    void set_highlight_text(bool highlight);
#endif
};

#endif
