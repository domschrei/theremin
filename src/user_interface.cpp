#include <iostream>
#include <string>
#include <vector>
#include <iomanip> // setprecision
#include <sstream> // stringstream

#include "const.h"
#include "user_interface.hpp"
#include "music_util.hpp"

/*
 * Initial input and video settings
 */
void UserInterface::setup(Configuration* cfg, WaveSynth* synth, Audio* audio) {
    
    this->cfg = cfg;
    this->synth = synth;
    this->audio = audio;
    
#ifdef THEREMIN_GUI
    // Initialize SDL video AND audio mode
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
#else
    // Only initialize SDL audio mode
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "Could not initialise SDL: %s\n", SDL_GetError());
        exit(-1);
    }
    
    // Initialize curses
    initscr();
    cbreak(); // one character at a time
    noecho(); // do not echo typed stuff
    nodelay(stdscr, TRUE); // non-blocking keypress polling
    keypad(stdscr, TRUE); // also capture non-character keypresses
    start_color(); // enable basic colors
    scrollok(stdscr,FALSE);
    curs_set(0); // make cursor invisible
#endif
}

/*
 * Returns a vector of strings. Those strings determine
 * which actions were triggered by containing the character
 * of the according key which has been pressed.
 */
std::vector<std::string> UserInterface::poll_events() {
    
    std::vector<std::string> actions;
    
#ifdef THEREMIN_GUI
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
#else
    int ch;
    while ((ch = getch()) != ERR) {
        actions.push_back(std::string(1, ch));
    }
#endif
    
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

#ifdef THEREMIN_GUI
    // Graphical window
    
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
    
    draw_chords();
        
    // Publish rendered surface
    SDL_RenderPresent(renderer);
    
#else
    // Terminal interface
    
    // Initialize
    clear();
    attroff(A_STANDOUT);
    
    // Check if terminal has necessary minimum size
    int width = 56; int height = 16;
    if (getmaxx(stdscr) < width || getmaxy(stdscr) < height) {
        printw("Terminal too small! (minimum %ix%i)", width, height);
        return;
    }
    
    // Box around contents with title
    print_box(width, height);
        
    // First column
    move(2, 2);
    
    print_current_volume();
    print_current_note();
    print_current_chords(); 
    print_help_text();
        
    // Second column
    move(2, 30);
    
    print_buttons();
    
    refresh();
    
#endif
}

#ifdef THEREMIN_GUI
/*
 * Draws a label ("pedal") for each possible effect.
 * The label will appeared "pressed" if a corresponding
 * action is being triggered.
 */
void UserInterface::draw_effect_labels() {
    
    /* Assemble strings describing the current effects */
    
    std::string strSustainNote = "[" + cfg->str(ACTION_SUSTAIN_NOTE) + "] ";
    if (synth->is_secondary_frequency_active()) {
        strSustainNote += "Playing 2nd note";
    } else {
        strSustainNote += "Playing single note";
    }
    
    std::string strOctaveUp = "[" + cfg->str(ACTION_OCTAVE_UP) + "] ";
    if (synth->is_octave_offset()) {
        strOctaveUp += "All notes octaved";
    } else {
        strOctaveUp += "All notes regular";
    }
    
    std::string strTremolo = "[" + cfg->str(ACTION_TREMOLO) + "] ";
    if (synth->is_tremolo_enabled()) {
        strTremolo += "Tremolo enabled";
    } else {
        strTremolo += "Tremolo disabled";
    }
    
    std::string strWaveform = "[" + cfg->str(ACTION_CHANGE_WAVEFORM) + "] ";
    strWaveform += "Wave: " + synth->get_waveform();
    
    std::string strAutotune = "[" 
            + cfg->str(ACTION_AUTOTUNE_NONE) 
            + "/" + cfg->str(ACTION_AUTOTUNE_SMOOTH) 
            + "/" + cfg->str(ACTION_AUTOTUNE_FULL) + "] ";
    strAutotune += std::string("Autotune: ") + synth->get_autotune_mode();
    
    std::string strRecording = "[" + cfg->str(ACTION_RECORDING_REPLAYING) + "] ";
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
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &noteRect);
    round_corners(noteRect);
    
    // Calculate the currently nearest note and the according color
    int lowerNoteIdx = MusicUtil::get_nearest_lower_note_index(synth->frequency);
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

void UserInterface::draw_chords() {
    
    int noteIdx = MusicUtil::get_nearest_note_index(synth->frequency);
    std::string maj1 = "[" + cfg->str(ACTION_CHORD_MAJOR_1) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_1, CHORD_KEY_MAJOR);
    std::string maj3 = "[" + cfg->str(ACTION_CHORD_MAJOR_3) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_3, CHORD_KEY_MAJOR);
    std::string maj5 = "[" + cfg->str(ACTION_CHORD_MAJOR_5) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_5, CHORD_KEY_MAJOR);
    std::string min1 = "[" + cfg->str(ACTION_CHORD_MINOR_1) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_1, CHORD_KEY_MINOR);
    std::string min3 = "[" + cfg->str(ACTION_CHORD_MINOR_3) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_3, CHORD_KEY_MINOR);
    std::string min5 = "[" + cfg->str(ACTION_CHORD_MINOR_5) + "] " 
            + MusicUtil::get_chord_name(noteIdx, CHORD_MODE_5, CHORD_KEY_MINOR);
    std::string clear = "[" + cfg->str(ACTION_CHORD_CLEAR) + "] Clear chords";
    
    draw_pedal(maj1.c_str(), 20, 30, 60, 30, false);
    draw_pedal(maj3.c_str(), 20, 65, 60, 30, false);
    draw_pedal(maj5.c_str(), 20, 100, 60, 30, false);
    draw_pedal(min1.c_str(), 85, 30, 60, 30, false);
    draw_pedal(min3.c_str(), 85, 65, 60, 30, false);
    draw_pedal(min5.c_str(), 85, 100, 60, 30, false);
    draw_pedal(clear.c_str(), 20, 135, 125, 30, false);
    
    SDL_Color textColor = {0, 0, 0, 255};
    std::string currentChord = synth->get_current_chord_name();
    currentChord = "Current chord: " + (currentChord != "" ? currentChord : "none");
    draw_text(currentChord.c_str(), 20, 170, textColor, sans);
}

/*
 * Paints a single effect label ("pedal") with the given text 
 * at the given position.
 * Will be rendered "negative" (i.e. black background, white text)
 * if isPressed is true.
 */
void UserInterface::draw_pedal(const char* text, int x, int y, bool isPressed) {
    
    draw_pedal(text, x, y, 220, 30, isPressed);
}

void UserInterface::draw_pedal(const char* text, int x, int y, int w, int h, bool isPressed) {
    
    SDL_Rect rect;
    rect.x = x; rect.y = y; rect.w = w; rect.h = h;
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

#else

void UserInterface::print_box(int width, int height) {
    
    // Box
    move(0, 0);
    addch(ACS_ULCORNER);
    move(0, width);
    addch(ACS_URCORNER);
    move(height, 0);
    addch(ACS_LLCORNER);
    move(height, width);
    addch(ACS_LRCORNER);
    move(0, 1);
    hline(ACS_HLINE, width-1);
    move(height, 1);
    hline(ACS_HLINE, width-1);
    move(1, 0);
    vline(ACS_VLINE, height-1);
    move(1, width);
    vline(ACS_VLINE, height-1);
    
    // Title
    move(0, 2);
    print_and_wrap("THERESA - THEREmin-like Sensor Application");
}

void UserInterface::print_current_note() {
    
    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);
        
    // Calculate note and its correction
    int lower_note_idx = MusicUtil::get_nearest_lower_note_index(synth->frequency);
    MusicUtil::frequency_correction correction = MusicUtil::get_error_of_frequency(
        synth->get_normalized_frequency(synth->frequency), 
        synth->get_normalized_frequency(NOTES[lower_note_idx]), 
        synth->get_normalized_frequency(NOTES[lower_note_idx+1]));
    float error = correction.rel_error;
    int note_idx;
    if (correction.lower_note_nearer) note_idx = lower_note_idx;
    else note_idx = lower_note_idx + 1;
    
    // Played note
    print_and_wrap("CURRENT NOTE");
    int curx = getcurx(stdscr);
    printw("[[ ");
    if (error > 0.3) attron(COLOR_PAIR(3));
    else if (error > 0.15) attron(COLOR_PAIR(2));
    else attron(COLOR_PAIR(1));
    printw(NOTE_NAMES[note_idx]);
    attroff(COLOR_PAIR(1)); attroff(COLOR_PAIR(2)); attroff(COLOR_PAIR(3));
    move(getcury(stdscr), curx + 7);
    std::stringstream stream;
    stream << std::fixed << std::setprecision(2) << error;
    print_and_wrap(" ]] err: " + stream.str());
    move(getcury(stdscr), curx);
    print_linebreak();
}

void UserInterface::print_current_volume() {

    print_and_wrap("VOLUME");
    int vol_width = 20;
    float rel_volume = synth->volume / cfg->i(MAX_VOLUME);
    int filled_symbols = (int) (rel_volume * vol_width);
    int curx = getcurx(stdscr);
    printw("[");
    if (filled_symbols > 0) 
        hline(ACS_BLOCK, filled_symbols);
    move(getcury(stdscr), getcurx(stdscr) + vol_width);
    printw("]");
    move(getcury(stdscr)+1, curx);
    print_linebreak();
}

void UserInterface::print_current_chords() {
    
    int note_idx = MusicUtil::get_nearest_note_index(synth->frequency);
    std::string maj1 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_1, CHORD_KEY_MAJOR);
    std::string maj3 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_3, CHORD_KEY_MAJOR);
    std::string maj5 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_5, CHORD_KEY_MAJOR);
    std::string min1 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_1, CHORD_KEY_MINOR);
    std::string min3 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_3, CHORD_KEY_MINOR);
    std::string min5 = MusicUtil::get_chord_name(note_idx, CHORD_MODE_5, CHORD_KEY_MINOR);
    std::string clear = "[" + cfg->str(ACTION_CHORD_CLEAR) + "] Clear chords";
    print_and_wrap("CHORDS");
    std::string currentChord = synth->get_current_chord_name();
    int curx = getcurx(stdscr);
    set_highlight_text(currentChord == maj1); printw(("[" + cfg->str(ACTION_CHORD_MAJOR_1) + "] " + maj1).c_str());
    move(getcury(stdscr), curx+8);
    set_highlight_text(currentChord == maj3); printw(("[" + cfg->str(ACTION_CHORD_MAJOR_3) + "] " + maj3).c_str());
    move(getcury(stdscr), curx+16);
    set_highlight_text(currentChord == maj5); printw(("[" + cfg->str(ACTION_CHORD_MAJOR_5) + "] " + maj5).c_str());
    move(getcury(stdscr)+1, curx);
    set_highlight_text(currentChord == min1); printw(("[" + cfg->str(ACTION_CHORD_MINOR_1) + "] " + min1).c_str());
    move(getcury(stdscr), curx+8);
    set_highlight_text(currentChord == min3); printw(("[" + cfg->str(ACTION_CHORD_MINOR_3) + "] " + min3).c_str());
    move(getcury(stdscr), curx+16);
    set_highlight_text(currentChord == min5); printw(("[" + cfg->str(ACTION_CHORD_MINOR_5) + "] " + min5).c_str());
    move(getcury(stdscr)+1, curx);
    set_highlight_text(false);
    print_and_wrap(clear);
    print_linebreak();
}

void UserInterface::print_help_text() {
    
    print_and_wrap(std::string(HELP_TEXT_1) + " " + HELP_TEXT_2);
    print_and_wrap(std::string(HELP_TEXT_3) + " " + HELP_TEXT_4);
    print_linebreak();
}

void UserInterface::print_buttons() {
    
    set_highlight_text(synth->is_secondary_frequency_active());
    print_and_wrap("[" + cfg->str(ACTION_SUSTAIN_NOTE) + "] Sustain note ");
    set_highlight_text(synth->is_octave_offset());
    print_and_wrap("[" + cfg->str(ACTION_OCTAVE_UP)    + "] Add octave   ");
    set_highlight_text(synth->is_tremolo_enabled());
    print_and_wrap("[" + cfg->str(ACTION_TREMOLO)      + "] Add tremolo  ");
    set_highlight_text(false);
    print_linebreak();
    
    print_and_wrap("["  + cfg->str(ACTION_AUTOTUNE_NONE) + "/" 
                + cfg->str(ACTION_AUTOTUNE_SMOOTH) + "/" 
                + cfg->str(ACTION_AUTOTUNE_FULL) + "] Autotune: " + synth->get_autotune_mode());
    print_and_wrap("[" + cfg->str(ACTION_CHANGE_WAVEFORM) + "] Waveform: " + synth->get_waveform());
    if (audio->is_recording()) attron(A_BLINK);
    set_highlight_text(audio->is_replaying() || audio->is_recording());
    print_and_wrap("[" + cfg->str(ACTION_RECORDING_REPLAYING) + "] Record / replay ");
    attroff(A_BLINK);
    print_linebreak();
}

void UserInterface::print_and_wrap(std::string str) {
    int xpos = getcurx(stdscr);
    printw(str.c_str()); move(getcury(stdscr)+1, xpos);
}

void UserInterface::print_linebreak() {
    move(getcury(stdscr)+1, getcurx(stdscr));
}

void UserInterface::set_highlight_text(bool highlight) {
    if (highlight)
        attron(A_STANDOUT);
    else
        attroff(A_STANDOUT);
}


#endif

/*
 * Free recources when the program is closed.
 */
void UserInterface::clean_up() {

#ifdef THEREMIN_GUI
    SDL_DestroyRenderer(renderer);
#else
    endwin(); // End curses
#endif
    SDL_Quit();
}
