#include <iostream>
#include <string>
#include <vector>

#include "const.h"
#include "configuration.hpp"
#include "wave_synth.hpp"
#include "user_interface.hpp"
#include "audio.hpp"
#include "sensor_input.hpp"

WaveSynth synth;
UserInterface userInterface;
Audio audio;
SensorInput sensorInput;

Configuration* cfg;

int periodInputMouse;
int periodInputSensor;
int periodInputGeneral;
int periodDisplayRefresh;

/*
 * To be called at program exit.
 */
void finish() {
    
    audio.set_exiting(true);
    
    if (cfg->str(INPUT_DEVICE) == INPUT_DEVICE_SENSOR) {
        sensorInput.finish();
        std::cout << "Finished sensor connection." << std::endl;
    }
    
    userInterface.clean_up();
    
    delete cfg;
}

/*
 * Help function called at each n-th iteration processing the secondary
 * (i.e. keyboard press) input events.
 */
void process_input(std::vector<std::string> actions) {
    
    for (int actionIdx = 0; actionIdx < actions.size(); actionIdx++) {
        std::string action = actions[actionIdx];
        
        if (action == cfg->str(ACTION_SUSTAIN_NOTE)) {
            if (!synth.is_secondary_frequency_active()) {
                synth.set_secondary_frequency(synth.frequency);
            } else {
                synth.set_secondary_frequency(0.0);
            }
            
        } else if (action == cfg->str(ACTION_OCTAVE_UP)) {
            if (!synth.is_octave_offset()) {
                synth.set_octave_offset(true);
            } else {
                synth.set_octave_offset(false);
            }
            
        } else if (action == cfg->str(ACTION_CHANGE_WAVEFORM)) {
            synth.switch_waveform();
            
        } else if (action == cfg->str(ACTION_AUTOTUNE_NONE)) {
            synth.set_autotune_mode(AUTOTUNE_NONE);
            
        } else if (action == cfg->str(ACTION_AUTOTUNE_SMOOTH)) {
            synth.set_autotune_mode(AUTOTUNE_SMOOTH);
            
        } else if (action == cfg->str(ACTION_AUTOTUNE_FULL)) {
            synth.set_autotune_mode(AUTOTUNE_FULL);
            
        } else if (action == cfg->str(ACTION_TREMOLO)) {
            synth.set_tremolo(!synth.is_tremolo_enabled());
        
        } else if (action == cfg->str(ACTION_RECORDING_REPLAYING)) {
            if (!audio.is_recording() && !audio.is_replaying()) {
                audio.start_recording();
            } else if (audio.is_recording() && !audio.is_replaying()) {
                audio.stop_recording();
                audio.start_replaying();
            } else if (!audio.is_recording()) {
                audio.stop_replaying();
            } else {
                std::cerr << "Illegal recording/replaying configuration"
                        " reached." << std::endl;
                exit(1);
            }
            
        } else if (action == cfg->str(ACTION_CHORD_MAJOR_1)) {
            synth.set_chord_notes(CHORD_MODE_1, CHORD_KEY_MAJOR);
        } else if (action == cfg->str(ACTION_CHORD_MAJOR_3)) {
            synth.set_chord_notes(CHORD_MODE_3, CHORD_KEY_MAJOR);
        } else if (action == cfg->str(ACTION_CHORD_MAJOR_5)) {
            synth.set_chord_notes(CHORD_MODE_5, CHORD_KEY_MAJOR);
        } else if (action == cfg->str(ACTION_CHORD_MINOR_1)) {
            synth.set_chord_notes(CHORD_MODE_1, CHORD_KEY_MINOR);
        } else if (action == cfg->str(ACTION_CHORD_MINOR_3)) {
            synth.set_chord_notes(CHORD_MODE_3, CHORD_KEY_MINOR);
        } else if (action == cfg->str(ACTION_CHORD_MINOR_5)) {
            synth.set_chord_notes(CHORD_MODE_5, CHORD_KEY_MINOR);
        } else if (action == cfg->str(ACTION_CHORD_CLEAR)) {
            synth.clear_child_notes();
        }
    }
}

/*
 * Called every iteration. Processes input events, synthesizes audio
 * and refreshes the graphical display.
 */
void main_loop(int *t) {
    
    /*
     * Process primary input to adjust volume and frequency
     */
    if (cfg->str(INPUT_DEVICE) == INPUT_DEVICE_MOUSE) {
        
        // Grab input by mouse cursor
        if (*t % periodInputMouse == 0) {
            float x_value = 0, y_value = 0;
            userInterface.last_cursor_position(&x_value, &y_value);
            synth.update_frequency(y_value);
            synth.update_volume(x_value);
        }
        
    } else if (cfg->str(INPUT_DEVICE) == INPUT_DEVICE_SENSOR) {
        
        // Grab input by Tinkerforge sensors
        if (*t % periodInputSensor == 0) {
            double value = 0.0;
            bool valueOk;
            valueOk = sensorInput.volume_value(&value);
            if (valueOk) {
                synth.update_volume(value);
            }
            valueOk = sensorInput.frequency_value(&value);
            if (valueOk) {
                synth.update_frequency(value);
            }
        }
        
    } else {
        std::cerr << "No input device specified." << std::endl;
        exit(1);
    }    
    
    /*
    * Process secondary input (by keyboard or foot switch)
    */
    if (*t % periodInputGeneral == 0) {
        process_input(userInterface.poll_events());
    }
    
    /*
     * Create a new audio sample and offer it to the SDL buffer.
     * If it is being declined, move one step backwards
     * to keep the waveform continuous.
     */
    if (audio.new_sample(synth.wave(*t))) {
        // Update the current volume 
        // in direction of the current volume target
        synth.volume_tick();
    } else {
        (*t)--;
    }

    /*
     * Start playing if the audio buffer is full for the first time, 
     * or after it stopped working
     */
    if (audio.is_buffer_full() && !audio.is_playing()) {
        audio.start_playing();
    }
    
    /*
     * Refresh the drawn surface at some times, if enabled
     */
    if (cfg->b(REALTIME_DISPLAY) && (*t % periodDisplayRefresh == 0)) {
        userInterface.refresh_surface();
    }
    
    /*
     * Don't cause an integer overflow (might happen if the application
     * runs for several hours with sufficient sample rate)
     */
    if (*t == INT32_MAX) {
         *t = 0;
    }
}

int main(int argc, const char* argv[]) 
{
    // Load configuration
    cfg = new Configuration();
    cfg->load();
    
    // Calculate periods for various tasks
    int sampleRate = cfg->i(SAMPLE_RATE);
    periodInputMouse = sampleRate / cfg->i(TASK_FREQUENCY_INPUT_MOUSE);
    periodInputSensor = sampleRate / cfg->i(TASK_FREQUENCY_INPUT_SENSOR);
    periodInputGeneral = sampleRate / cfg->i(TASK_FREQUENCY_INPUT_GENERAL);
    periodDisplayRefresh = sampleRate / cfg->i(TASK_FREQUENCY_DISPLAY_REFRESH);
    
    // Basic input (i.e. mouse and keys / footswitch)
    // and graphical output
    userInterface.setup(cfg, &synth, &audio);
    
    // Sensor input
    if (cfg->str(INPUT_DEVICE) == INPUT_DEVICE_SENSOR) {
        sensorInput.setup_sensors(cfg);
    }
    
    // Audio output stuff
    audio.setup_audio(cfg);
    
    // Wave synthesizer
    synth.init(cfg);
    
    // Program exit callback
    atexit(finish);
    
    // Run main loop until closed
    for (int t = 1; ; t++) {
        main_loop(&t);
    }
}
