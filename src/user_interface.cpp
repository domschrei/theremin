#include <iostream>
#include <string>
#include <vector>

#include "user_interface.hpp"

/*
 * Initial input and video settings
 */
void UserInterface::setup(Configuration* cfg, WaveSynth* synth, Audio* audio) {
    
    this->cfg = cfg;
    this->synth = synth;
    this->audio = audio;
    
    // Initialize SDL video mode
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }
    
    // Create a window
    window = SDL_CreateWindow("Theremin",
                          SDL_WINDOWPOS_UNDEFINED,
                          SDL_WINDOWPOS_UNDEFINED,
                          window_w, window_h, 0 );
    if (!window) {
        fprintf(stderr, "Couldn't create window: %s\n",
                        SDL_GetError());
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, 0);
    
    // Initialize TTF handling
    TTF_Init();
    sans = TTF_OpenFont("LiberationSans-Regular.ttf", 16);
    sansLarge = TTF_OpenFont("LiberationSans-Regular.ttf", 30);
    
    // Draw some one-time replacement for the real-time display, 
    // if the latter is disabled
    if (!cfg->b(REALTIME_DISPLAY)) {
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
 * Returns a vector of strings. Those strings determine
 * which actions were triggered by containing the character
 * of the according key which has been pressed.
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

/*
 * Returns the last known position of the mouse cursor,
 * previously checked with poll_events().
 */
void UserInterface::last_cursor_position(float *x, float *y) {
    
    *x = (float) mouse_x / window_w;
    *y = (float) (window_h - mouse_y) / window_h;
} 

/*
 * Repaints the surface according to the current information
 * of the WaveSynth instance which this class has access to.
 */
void UserInterface::refresh_surface() {
    
    // White background
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    
    // Status bars visualizing the current frequency and volume
    draw_status_bars();
        
    // Labels of effects
    draw_effect_labels();   
    
    // Current note display with semantic color
    draw_note_display();
        
    // Help text
    draw_help_text();
        
    // Publish rendered surface
    SDL_RenderPresent(renderer);
}

/*
 * Draws a label ("pedal") for each possible effect.
 * The label will appeared "pressed" if a corresponding
 * action is being triggered.
 */
void UserInterface::draw_effect_labels() {
    
    /* Assemble strings describing the current effects */
    
    std::string strSustainNote = "[";
    strSustainNote += cfg->str(ACTION_SUSTAIN_NOTE);
    strSustainNote += "] ";
    if (synth->is_secondary_frequency_active()) {
        strSustainNote += "Playing 2nd note";
    } else {
        strSustainNote += "Playing single note";
    }
    
    std::string strOctaveUp = "[";
    strOctaveUp += cfg->str(ACTION_OCTAVE_UP);
    strOctaveUp += "] ";
    if (synth->is_octave_offset()) {
        strOctaveUp += "All notes octaved";
    } else {
        strOctaveUp += "All notes regular";
    }
    
    std::string strTremolo = "[";
    strTremolo += cfg->str(ACTION_TREMOLO);
    strTremolo += "] ";
    if (synth->is_tremolo_enabled()) {
        strTremolo += "Tremolo enabled";
    } else {
        strTremolo += "Tremolo disabled";
    }
    
    std::string strWaveform = "[";
    strWaveform += cfg->str(ACTION_CHANGE_WAVEFORM);
    strWaveform += "] ";
    strWaveform += "Wave: " + synth->get_waveform();
    
    std::string strAutotune = "[";
    strAutotune += cfg->str(ACTION_AUTOTUNE_NONE) 
            + "/" + cfg->str(ACTION_AUTOTUNE_SMOOTH) 
            + "/" + cfg->str(ACTION_AUTOTUNE_FULL);
    strAutotune += "] ";
    strAutotune += std::string("Autotune: ") + synth->get_autotune_mode();
    
    std::string strRecording = "[";
    strRecording += cfg->str(ACTION_RECORDING_REPLAYING);
    strRecording += "] ";
    if (audio->is_replaying()) {
        strRecording += "Replaying";
    } else if (audio->is_recording()) {
        strRecording += "Recording";
    } else {
        strRecording += "Not replaying/recording";
    }
        
    // Draw the effect labels ("pedals")
    draw_pedal(strSustainNote.c_str(), 350, 15, synth->is_secondary_frequency_active());
    draw_pedal(strOctaveUp.c_str(), 350, 55, synth->is_octave_offset());
    draw_pedal(strTremolo.c_str(), 350, 95, synth->is_tremolo_enabled());
    draw_pedal(strWaveform.c_str(), 350, 135, lastWaveform != synth->get_waveform());
    draw_pedal(strAutotune.c_str(), 350, 175, lastAutotuneMode != synth->get_autotune_mode());
    draw_pedal(strRecording.c_str(), 350, 215, audio->is_replaying() || audio->is_recording());
    
    // Remember current state for next cycle
    lastWaveform = synth->get_waveform();
    lastAutotuneMode = synth->get_autotune_mode();
}

/*
 * Paints a black rectangle with the currently playing note in it.
 * The note will appear brighter the better the frequency is aligned.
 */
void UserInterface::draw_note_display() {
    
    double freqNormalized = synth->get_normalized_frequency(synth->frequency);
    
    // Draw black rectangle
    SDL_Rect noteRect;
    noteRect.x = 190; noteRect.y = 70;
    noteRect.w = 70; noteRect.h = 70;
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect(renderer, &noteRect);
    round_corners(noteRect);
    
    // Calculate the currently nearest note and the according color
    int lowerNoteIdx = synth->get_nearest_lower_note_index();
    int noteIdx = lowerNoteIdx;
    double lowerNoteFreqNorm = synth->get_normalized_frequency(NOTES[lowerNoteIdx]);
    float error = 0.0;
    SDL_Color noteColor = {220, 220, 255, 255};
    if (lowerNoteIdx + 1 < sizeof(NOTE_NAMES) / sizeof(*NOTE_NAMES)) {
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
    
    // Display the note name
    draw_text(NOTE_NAMES[noteIdx], 195, 90, noteColor, sansLarge);
}

/*
 * Paints two status bars which visualize the current frequency
 * and volume relative to their minima and maxima.
 */
void UserInterface::draw_status_bars() {
    
    SDL_Color textColor = {0, 0, 0, 255};
    
    // "Volume" label
    draw_text("Volume", 30, window_h - 80, textColor, sans);
        
    // Volume bar
    SDL_Rect progressVolume;
    progressVolume.x = 15; progressVolume.y = window_h - 45; 
    progressVolume.w = window_w - 30; progressVolume.h = 30;
    draw_progress(progressVolume, synth->volume / cfg->i(MAX_VOLUME), true);
    
    // "Frequency" label
    draw_text("Frequency", 190, 30, textColor, sans);
        
    // Frequency bar
    SDL_Rect progressFrequency;
    progressFrequency.x = window_w * 0.5 - 15; progressFrequency.y = 15; 
    progressFrequency.w = 30; progressFrequency.h = window_h - 100;
    double freqNormalized = synth->get_normalized_frequency(synth->frequency);
    draw_progress(progressFrequency, freqNormalized, false);
}

/*
 * Displays some lines of help text.
 */
void UserInterface::draw_help_text() {
    
    SDL_Color textColor = {0, 0, 0, 255};
    draw_text(HELP_TEXT_1, 350, 270, textColor, sans);
    draw_text(HELP_TEXT_2, 350, 290, textColor, sans);
    draw_text(HELP_TEXT_3, 350, 320, textColor, sans);
    draw_text(HELP_TEXT_4, 350, 340, textColor, sans);
}

/*
 * Paints a single effect label ("pedal") with the given text 
 * at the given position.
 * Will be rendered "negative" (i.e. black background, white text)
 * if isPressed is true.
 */
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

/*
 * Displays text.
 */
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

/*
 * Paints a progress bar inside the given rectangle which is filled
 * to a share of the given float (0 <= f <= 1).
 * If inReadDirection is true, the bar will fill from left to right
 * or from top to bottom, depending on which side of the rectangle
 * is larger.
 * If inReadDirection is false, it will fill towards the opposite
 * direction.
 */
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

/*
 * Minimalistically rounds corners by adding white pixels to the edges
 * of the given rectangle.
 */
void UserInterface::round_corners(SDL_Rect rect) {
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y);
    SDL_RenderDrawPoint(renderer, rect.x, rect.y + rect.h - 1);
    SDL_RenderDrawPoint(renderer, rect.x + rect.w - 1, rect.y + rect.h - 1);
}

/*
 * Free recources when the program is closed.
 */
void UserInterface::clean_up() {
    
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
