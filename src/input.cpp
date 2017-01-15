#include <iostream>

#include "input.h"

/*
 * Returns an array of bools. At index i of this array,
 * the bool determines whether the input with ID i has
 * occured (see Input::INPUT_*).
 */
bool* Input::poll_events()
{    
    static bool results[13];
    for (int i = 0; i < 13; i++) {
        results[i] = false;
    }
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        
        switch (event.type) {
        /*
         * This includes a hotfix such that a release and press of a key
         * at the same time slice will negate each other.
         */
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_a) {
                results[INPUT_PRESS_A] = true;
            } else if (event.key.keysym.sym == SDLK_b) {
                results[INPUT_PRESS_B] = true;
            } else if (event.key.keysym.sym == SDLK_c) {
                results[INPUT_PRESS_C] = true;
            } else if (event.key.keysym.sym == SDLK_1) {
                results[INPUT_PRESS_1] = true;
            } else if (event.key.keysym.sym == SDLK_2) {
                results[INPUT_PRESS_2] = true;
            } else if (event.key.keysym.sym == SDLK_3) {
                results[INPUT_PRESS_3] = true;
            }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_a) {
                results[INPUT_RELEASE_A] = true;
            } else if (event.key.keysym.sym == SDLK_b) {
                results[INPUT_RELEASE_B] = true;
            } else if (event.key.keysym.sym == SDLK_c) {
                results[INPUT_RELEASE_C] = true;
            } 
            break;
        case SDL_MOUSEMOTION:
            mouse_x = event.motion.x;
            mouse_y = event.motion.y;
            break;
        case SDL_QUIT:
            exit(0);
        }
    }
    
    return results;
}

void Input::refresh_surface(WaveSynth* synth) {
    
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
    draw_progress(progressVolume, synth->volume / MAX_VOLUME, true);
        
    // "Frequency" label
    draw_text("Frequency", 190, 30, textColor, sans);
        
    // Frequency bar
    SDL_Rect progressFrequency;
    progressFrequency.x = window_w * 0.5 - 15; progressFrequency.y = 15; 
    progressFrequency.w = 30; progressFrequency.h = window_h - 100;
    double freqNormalized = synth->get_normalized_frequency(synth->frequency);
    draw_progress(progressFrequency, freqNormalized, false);
        
    // Labels of pedals
    const char* textA;
    const char* textB;
    const char* textC;
    if (synth->is_secondary_frequency_active()) {
        textA = "Pedal A: Playing 2nd note";
    } else {
        textA = "Pedal A: Playing single note";
    }
    if (synth->is_octave_offset()) {
        textB = "Pedal B: All notes octaved";
    } else {
        textB = "Pedal B: All notes regular";
    }
    // Pedal C: display index and name of the current wave
    std::string waveform = std::to_string(synth->get_waveform());
    std::string waveText = std::string("Pedal C: Wave ") + waveform;
    waveText = waveText + " \"";
    waveText = waveText + WAVE_NAMES[synth->get_waveform()];
    waveText = waveText + "\"";
    textC = waveText.c_str();
    // Autotune "pedal"
    std::string autotuneIdx = std::to_string(synth->get_autotune_mode());
    std::string autotuneText = std::string("Autotune: ") + autotuneIdx;
    if (synth->get_autotune_mode() == AUTOTUNE_NONE) {
        autotuneText += std::string(" (none)");
    } else if (synth->get_autotune_mode() == AUTOTUNE_SMOOTH) {
        autotuneText += std::string(" (smooth)");
    } else if (synth->get_autotune_mode() == AUTOTUNE_FULL) {
        autotuneText += std::string(" (full)");
    }
    const char* textAutotune = autotuneText.c_str();
        
    // Draw pedals
    draw_pedal(textA, 350, 30, synth->is_secondary_frequency_active());
    draw_pedal(textB, 350, 70, synth->is_octave_offset());
    draw_pedal(textC, 350, 110, lastWaveform != synth->get_waveform());
    draw_pedal(textAutotune, 350, 160, lastAutotuneMode != synth->get_autotune_mode());
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
    if (lowerNoteIdx + 1 < NUM_NOTES) {
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
    draw_text(HELP_TEXT_1, 350, 220, textColor, sans);
    draw_text(HELP_TEXT_2, 350, 240, textColor, sans);
    draw_text(HELP_TEXT_3, 350, 270, textColor, sans);
    draw_text(HELP_TEXT_4, 350, 290, textColor, sans);
    draw_text(HELP_TEXT_5, 350, 320, textColor, sans);
    draw_text(HELP_TEXT_6, 350, 340, textColor, sans);
        
    // Publish rendered surface
    SDL_RenderPresent(renderer);
}

void Input::draw_pedal(const char* text, int x, int y, bool isPressed) {
    
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

void Input::draw_text(const char* text, int x, int y, SDL_Color color, TTF_Font* font) {

    SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect textRect;
    textRect.x = x; textRect.y = y; 
    textRect.w = surfaceMessage->clip_rect.w; textRect.h = surfaceMessage->clip_rect.h; 
    SDL_RenderCopy(renderer, message, &(surfaceMessage->clip_rect), &textRect);
    
    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

void Input::draw_progress(SDL_Rect rect, float share, bool inReadDirection) {
    
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

void Input::round_corners(SDL_Rect rect) {
    
    // Minimalistically rounded corners
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y + rect.h - 1);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y + rect.h - 1);
}

/*
 * Initial input and video settings
 */
void Input::setup() {
    
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
}

void Input::fetch_input(float *x, float *y) {
    
    *x = (float) mouse_x / 1920;
    *y = (float) (1080 - mouse_y) / 1080;
} 

void Input::clean_up() {
    
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
