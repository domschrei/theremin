#include <iostream>
#include <string>
#include <vector>

#include "const.h"
#include "configuration.h"
#include "wave_synth.h"
#include "user_interface.h"
#include "audio.h"
#include "sensor_input.h"

WaveSynth synth;
UserInterface userInterface;
Audio audio;
SensorInput sensorInput;

Configuration* cfg;

int periodInputMouse;
int periodInputSensor;
int periodInputGeneral;
int periodDisplayRefresh;

void finish() {
    
    audio.set_exiting(true);
    
    if (cfg->str("input_device") == "sensor") {
        sensorInput.finish();
        std::cout << "Finished sensor connection." << std::endl;
    }
    
    userInterface.clean_up();
    
    delete cfg;
}

void main_loop(int *t) {
    
    /*
     * Process input to adjust volume and frequency
     */
    if (cfg->str("input_device") == "mouse") {
        
        // Grab input by mouse cursor
        if (*t % periodInputMouse == 0) {
            float x_value = 0, y_value = 0;
            userInterface.fetch_input(&x_value, &y_value);
            synth.update_frequency(y_value);
            synth.update_volume(x_value);
        }
        
    } else if (cfg->str("input_device") == "sensor") {
        
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
    
    if (*t % periodInputGeneral == 0) {
        /*
        * Process key input (by keyboard or foot switch)
        */
        std::vector<std::string> actions = userInterface.poll_events();
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
            }
        }
        
    }
    
    /*
     * Create a new audio sample and sacrifice it to the SDL audio gods.
     * If the gods decline the sacrifice, move one step backwards
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
    if (cfg->b("realtime_display") && (*t % periodDisplayRefresh == 0)) {
        userInterface.refresh_surface(&synth);
    }
    
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
    int sampleRate = cfg->i("sample_rate");
    periodInputMouse = sampleRate / cfg->i("task_frequency_input_mouse");
    periodInputSensor = sampleRate / cfg->i("task_frequency_input_sensor");
    periodInputGeneral = sampleRate / cfg->i("task_frequency_input_general");
    periodDisplayRefresh = sampleRate / cfg->i("task_frequency_display_refresh");
    
    // Basic input (i.e. mouse and keys / footswitch)
    // and graphical output
    userInterface.setup(cfg);
    
    // Sensor input
    if (cfg->str("input_device") == "sensor") {
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
