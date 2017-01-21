#include <iostream>

#include "config.h"
#include "wave_synth.h"
#include "user_interface.h"
#include "audio.h"
#include "sensor_input.h"

WaveSynth synth;
UserInterface userInterface;
Audio audio;
SensorInput sensorInput;

void finish() {
    
    audio.set_exiting(true);
    
    if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        sensorInput.finish();
        std::cout << "Finished sensor connection." << std::endl;
    }
    
    userInterface.clean_up();
}

void main_loop(int *t) {
    
    /*
     * Process input to adjust volume and frequency
     */
    if (INPUT_DEVICE == INPUT_DEVICE_MOUSE) {
        
        // Grab input by mouse cursor
        if (*t % PERIOD_INPUT_MOUSE == 0) {
            float x_value = 0, y_value = 0;
            userInterface.fetch_input(&x_value, &y_value);
            synth.update_frequency(y_value);
            synth.update_volume(x_value);
        }
        
    } else if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        
        // Grab input by Tinkerforge sensors
        if (*t % PERIOD_INPUT_SENSOR == 0) {
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
    
    if (*t % PERIOD_INPUT_GENERAL == 0) {
        /*
        * Process key input (by keyboard or foot switch)
        */
        bool* input_keys = userInterface.poll_events();
        
        // Adds a second tone to the audio output, if pressed
        if (input_keys[UserInterface::INPUT_PRESS_A]) {
            if (!synth.is_secondary_frequency_active()) {
                synth.set_secondary_frequency(synth.frequency);
            } else {
                synth.set_secondary_frequency(0.0);
            }
        }
        
        // Octaves the audio upwards, if pressed
        if (input_keys[UserInterface::INPUT_PRESS_B]) {
            if (!synth.is_octave_offset()) {
                synth.set_octave_offset(true);
            } else {
                synth.set_octave_offset(false);
            }
        }
        
        // Switches to the next waveform on each press
        if (input_keys[UserInterface::INPUT_PRESS_C]) {
            synth.switch_waveform();
        }
        
        // Autotune settings
        if (input_keys[UserInterface::INPUT_PRESS_1]) {
            synth.set_autotune_mode(AUTOTUNE_NONE);
        } else if (input_keys[UserInterface::INPUT_PRESS_2]) {
            synth.set_autotune_mode(AUTOTUNE_SMOOTH);
        } else if (input_keys[UserInterface::INPUT_PRESS_3]) {
            synth.set_autotune_mode(AUTOTUNE_FULL);
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
    if (REALTIME_DISPLAY && (*t % PERIOD_DISPLAY_REFRESH == 0)) {
        userInterface.refresh_surface(&synth);
    }
    
    if (*t == INT32_MAX) {
         *t = 0;
    }
}

int main(int argc, const char* argv[]) 
{
    // Basic input (i.e. mouse and keys / footswitch)
    // and graphical output
    userInterface.setup();
    
    // Sensor input
    if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        sensorInput.setup_sensors();
    }
    
    // Audio output stuff
    audio.setup_audio();
    
    // Program exit callback
    atexit(finish);
    
    // Run main loop until closed
    for (int t = 1; ; t++) {
        main_loop(&t);
    }
}
