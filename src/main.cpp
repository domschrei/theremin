#include <iostream>

#include "config.h"
#include "wave_synth.h"
#include "input.h"
#include "audio.h"
#include "sensor_input.h"

WaveSynth synth;
Input input;
Audio* audio;
SensorInput sensorInput;

void print_instructions() {
    
    if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        std::cout << "Use the Tinkerforge distance sensors to control frequency and volume." << std::endl;
    } else if (INPUT_DEVICE == INPUT_DEVICE_MOUSE) {
        std::cout << "Use your mouse cursor:\n" << std::endl;
        std::cout << "      Freq+      " << std::endl;
        std::cout << "        ^        " << std::endl;
        std::cout << "Vol- <     > Vol+" << std::endl;
        std::cout << "        v        " << std::endl;
        std::cout << "      Freq-      " << std::endl;
    }
}

void finish() {
    
    audio->exiting = true;
    
    if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        sensorInput.finish();
        std::cout << "Finished sensor connection." << std::endl;
    }
    
    input.clean_up();
}

void main_loop(int *t) {
    
    /*
     * Process input to adjust volume and frequency
     */
    if (INPUT_DEVICE == INPUT_DEVICE_MOUSE) {
        
        // Grab input by mouse cursor
        if (*t % PERIOD_INPUT_MOUSE == 0) {
            float x_value = 0, y_value = 0;
            input.fetch_input(&x_value, &y_value);
            synth.update_frequency(y_value);
            synth.update_volume(x_value);
        }
        
    } else if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        
        // Grab input by Tinkerforge sensors
        if (*t % PERIOD_INPUT_SENSOR == 0) {
            
            uint16_t value = 0;
            
            distance_us_get_distance_value(&(sensorInput.distanceFrequency), &value);
            // do not report values over the threshold
            if (value <= SENSOR_FREQ_MAX_VALUE) {
                // normalize the value to [0,1] and update the wave synthesizer
                synth.update_frequency(((double) value - SENSOR_FREQ_MIN_VALUE) 
                                / (SENSOR_FREQ_MAX_VALUE - SENSOR_FREQ_MIN_VALUE));
            }
            
            distance_us_get_distance_value(&(sensorInput.distanceVolume), &value);
            // cap values at the threshold
            if (value >= SENSOR_VOL_MAX_VALUE) {
                value = SENSOR_VOL_MAX_VALUE;
            }
            // normalize the value to [0,1] and update the wave synthesizer
            synth.update_volume(1 - ((double) value - SENSOR_VOL_MIN_VALUE) 
                            / (SENSOR_VOL_MAX_VALUE - SENSOR_VOL_MIN_VALUE));
        }
        
    } else {
        std::cerr << "No input device specified." << std::endl;
        exit(1);
    }    
    
    if (*t % PERIOD_INPUT_GENERAL == 1) {
        /*
        * Process key input (by keyboard or foot switch)
        */
        bool* input_keys = input.poll_events();
        
        // Adds a second tone to the audio output, if pressed
        if (input_keys[Input::INPUT_PRESS_A]) {
            if (!synth.is_secondary_frequency_active()) {
                synth.set_secondary_frequency(synth.frequency);
            } else {
                synth.set_secondary_frequency(0.0);
            }
        }
        
        // Octaves the audio upwards, if pressed
        if (input_keys[Input::INPUT_PRESS_B]) {
            if (!synth.is_octave_offset()) {
                synth.set_octave_offset(true);
            } else {
                synth.set_octave_offset(false);
            }
        }
        
        // Switches to the next waveform on each press
        if (input_keys[Input::INPUT_PRESS_C]) {
            synth.switch_waveform();
        }
        
        // Autotune settings
        if (input_keys[Input::INPUT_PRESS_1]) {
            synth.set_autotune_mode(AUTOTUNE_NONE);
        } else if (input_keys[Input::INPUT_PRESS_2]) {
            synth.set_autotune_mode(AUTOTUNE_SMOOTH);
        } else if (input_keys[Input::INPUT_PRESS_3]) {
            synth.set_autotune_mode(AUTOTUNE_FULL);
        }
    }
    
    /*
     * Create a new audio sample corresponding to the current audio attributes
     */
    if (!audio->is_buffer_full()) {
        audio->new_sample(synth.wave(*t));
    } else {
        (*t)--;
    }

    /*
     * Start playing if the audio buffer is full for the first time, 
     * or after it stopped working
     */
    if (audio->is_buffer_full() && !audio->isPlaying) {
        audio->start_playing();
    }
    
    /*
     * Refresh the drawn surface at some times, if enabled
     */
    if (REALTIME_DISPLAY && (*t % PERIOD_DISPLAY_REFRESH == 2)) {
        input.refresh_surface(&synth);
    }
    
    if (*t == INT32_MAX) {
         *t = 0;
    }
}

int main(int argc, const char* argv[]) 
{
    // Basic input (i.e. mouse and keys / footswitch)
    input.setup();
    
    // Sensor input
    if (INPUT_DEVICE == INPUT_DEVICE_SENSOR) {
        sensorInput.setup_sensors();
    }
    
    // Audio output stuff
    audio = Audio::get_std_audio();
    audio->setup_audio();
    
    // Program exit callback
    atexit(finish);
    
    // Run main loop until closed
    for (int t = 1; ; t++) {
        main_loop(&t);
    }
}
