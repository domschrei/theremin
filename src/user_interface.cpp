#include <iostream>
#include <string>
#include <vector>

#include "user_interface.h"

/*
 * Initial input and video settings
 */
void UserInterface::setup(Configuration* cfg) {
    
    this->cfg = cfg;
    
    // Initialize SDL video mode
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }
    
    // Create a window
    window = SDL_CreateWindow("Theremin",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          window_w, window_h, 
                          /*SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL*/ 0 );
    if (!window) {
        fprintf(stderr, "Couldn't create window: %s\n",
                        SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    TTF_Init();
    sans = TTF_OpenFont("LiberationSans-Regular.ttf", 16);
    sansLarge = TTF_OpenFont("LiberationSans-Regular.ttf", 30);
    
    if (!cfg->b("realtime_display")) {
        
        // Constant black text on white background
        SDL_Color black = {0, 0, 0, 255};
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        draw_text("THEREMIN", 20, 20, black, sansLarge);
        draw_text("Realtime display disabled.", 20, 60, black, sans);
        SDL_RenderPresent(renderer);
    }
}

/*
 * Returns an array of bools. At index i of this array,
 * the bool determines whether the input with ID i has
 * occured (see the constants INPUT_*).
 */
std::vector<std::string> UserInterface::poll_events() {
    
    std::vector<std::string> actions;
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
        case SDL_TEXTINPUT:
            actions.push_back(event.text.text);
            break;
        case SDL_MOUSEMOTION:
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            break;
        case SDL_QUIT:
            exit(0);
        }
    }
    
    return actions;
}

void UserInterface::fetch_input(float *x, float *y) {
    
    *x = (float) mouse_x / window_w;
    *y = (float) (window_h - mouse_y) / window_h;
} 

void UserInterface::refresh_surface(WaveSynth* synth) {
    
    // White background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // "Volume" label
    SDL_Color textColor = {0, 0, 0, 255};
    draw_text("Volume", 30, window_h - 80, textColor, sans);
        
    // Volume bar
    SDL_Rect progressVolume;
    progressVolume.x = 15; progressVolume.y = window_h - 45; 
    progressVolume.w = window_w - 30; progressVolume.h = 30;
    draw_progress(progressVolume, synth->volume / cfg->i("max_volume"), true);
        
    // "Frequency" label
    draw_text("Frequency", 190, 30, textColor, sans);
        
    // Frequency bar
    SDL_Rect progressFrequency;
    progressFrequency.x = window_w * 0.5 - 15; progressFrequency.y = 15; 
    progressFrequency.w = 30; progressFrequency.h = window_h - 100;
    double freqNormalized = synth->get_normalized_frequency(synth->frequency);
    draw_progress(progressFrequency, freqNormalized, false);
        
    // Labels of effects
    const char* textA;
    const char* textB;
    const char* textC;
    const char* textD;
    if (synth->is_secondary_frequency_active()) {
        textA = "Playing 2nd note";
    } else {
        textA = "Playing single note";
    }
    if (synth->is_octave_offset()) {
        textB = "All notes octaved";
    } else {
        textB = "All notes regular";
    }
    // Display index and name of the current wave
    std::string waveform = synth->get_waveform();
    std::string waveText = std::string("Wave: ") + waveform;
    textC = waveText.c_str();
    if (synth->is_tremolo_enabled()) {
        textD = "Tremolo enabled";
    } else {
        textD = "Tremolo disabled";
    }
    // Autotune "pedal"
    std::string autotuneMode = synth->get_autotune_mode();
    std::string autotuneText = std::string("Autotune: ") + autotuneMode;
    const char* textAutotune = autotuneText.c_str();
        
    // Draw pedals
    draw_pedal(textA, 350, 15, synth->is_secondary_frequency_active());
    draw_pedal(textB, 350, 55, synth->is_octave_offset());
    draw_pedal(textD, 350, 95, synth->is_tremolo_enabled());
    draw_pedal(textC, 350, 135, lastWaveform != synth->get_waveform());
    draw_pedal(textAutotune, 350, 175, lastAutotuneMode != synth->get_autotune_mode());
    lastWaveform = synth->get_waveform();
    lastAutotuneMode = synth->get_autotune_mode();
       
    // Note display square
    SDL_Rect noteRect;
    noteRect.x = 190; noteRect.y = 70;
    noteRect.w = 70; noteRect.h = 70;
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect(renderer, &noteRect);
    round_corners(noteRect);
        
    // Current note display with semantic color
    int lowerNoteIdx = synth->get_nearest_lower_note_index();
    int noteIdx = lowerNoteIdx;
    double lowerNoteFreqNorm = synth->get_normalized_frequency(NOTES[lowerNoteIdx]);
    float error = 0.0;
    SDL_Color noteColor = {220, 220, 255, 255};
    if (lowerNoteIdx + 1 < sizeof(NOTE_NAMES)/sizeof(NOTE_NAMES)) {
        double nextNoteFreqNorm = synth->get_normalized_frequency(NOTES[lowerNoteIdx + 1]);
        double freqDiffNorm = nextNoteFreqNorm - lowerNoteFreqNorm;
        float diffUpper = nextNoteFreqNorm - freqNormalized;
        float diffLower = freqNormalized - lowerNoteFreqNorm;
        error = 0.0;
        if (diffUpper == 0) {
            noteIdx++;
        } else if (diffLower <= diffUpper) {
            error = diffLower / (0.5 * freqDiffNorm);
        } else if (diffLower > diffUpper) {
            noteIdx++;
            error = diffUpper / (0.5 * freqDiffNorm);
        }
    } else {
        error = 0.0;
    }
    noteColor.r = 220 * (1 - error);
    noteColor.g = 220 * (1 - error);
    noteColor.b = 255 * (1 - error);
    draw_text(NOTE_NAMES[noteIdx], 195, 90, noteColor, sansLarge);
        
    // Help text
    draw_text(HELP_TEXT_1, 350, 270, textColor, sans);
    draw_text(HELP_TEXT_2, 350, 290, textColor, sans);
    draw_text(HELP_TEXT_3, 350, 320, textColor, sans);
    draw_text(HELP_TEXT_4, 350, 340, textColor, sans);
        
    // Publish rendered surface
    SDL_RenderPresent(renderer);
}

void UserInterface::draw_pedal(const char* text, int x, int y, bool isPressed) {
    
    SDL_Rect rect;
    rect.x = x; rect.y = y; rect.w = 220; rect.h = 30;
    SDL_Color colorText;
    if (isPressed) {
        colorText = {255, 255, 255, 255};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    } else {
        colorText = {0, 0, 0, 255};
        SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
    }
    SDL_RenderFillRect(renderer, &rect);
    round_corners(rect);
    
    draw_text(text, x + 5, y + 5, colorText, sans);
}

void UserInterface::draw_text(const char* text, int x, int y, SDL_Color color, TTF_Font* font) {

    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect textRect;
    textRect.x = x; textRect.y = y; 
    textRect.w = surfaceMessage->clip_rect.w; textRect.h = surfaceMessage->clip_rect.h; 
    SDL_RenderCopy(renderer, message, &(surfaceMessage->clip_rect), &textRect);
    
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void UserInterface::draw_progress(SDL_Rect rect, float share, bool inReadDirection) {
    
    bool horizontal = (rect.w > rect.h);
    
    share = std::min(share, 1.0f);
    share = std::max(share, 0.0f);
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_Rect loadedRect;
    
    if (horizontal) {
        loadedRect.w = std::round(share * rect.w);
        loadedRect.h = rect.h;
    } else {
        loadedRect.w = rect.w;
        loadedRect.h = std::round(share * rect.h);
    }
        
    if (inReadDirection) {
        loadedRect.x = rect.x;
        loadedRect.y = rect.y;
    } else {
        if (horizontal) {
            loadedRect.x = rect.x + rect.w - std::round(share * rect.w);
            loadedRect.y = rect.y;
        } else {
            loadedRect.x = rect.x;
            loadedRect.y = rect.y + rect.h - std::round(share * rect.h);
        }
    }
    
    SDL_SetRenderDrawColor(renderer, 150, 150, 255, 255);
    SDL_RenderFillRect(renderer, &loadedRect);
    
    round_corners(rect);
}

void UserInterface::round_corners(SDL_Rect rect) {
    
    // Minimalistically rounded corners
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y + rect.h - 1);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y + rect.h - 1);
}

void UserInterface::clean_up() {
    
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
