#include <stdio.h>
#include <functional>

#include "const.h"
#include "sensor_input.h"

/*
 * Connects to the sensor brick at localhost:4223 and to the
 * two expected distance sensors.
 */
void SensorInput::setup_sensors(Configuration* cfg) {
    
    // Create IP connection
    ipcon_create(&ipcon);

    // Create device object
    distance_us_create(&distanceFrequency, cfg->str(UID_FREQUENCY).c_str(), &ipcon);
    distance_us_create(&distanceVolume, cfg->str(UID_VOLUME).c_str(), &ipcon);

    // Connect to brickd
    if(ipcon_connect(&ipcon, cfg->str(HOST).c_str(), cfg->i(PORT)) < 0) {
        fprintf(stderr, "Could not connect to the sensors.\n"
        "Please make sure that the Brick daemon is running "
        "(`sudo brickd --daemon`) and that the master brick "
        "is connected to the PC.\n");
        exit(1);
    }

    // Turn off moving average (because it causes higher latency)
    distance_us_set_moving_average(&distanceFrequency, 0);
    distance_us_set_moving_average(&distanceVolume, 0);
}

/*
 * Writes the current frequency value into the given double, 
 * normalized to [0,1], if true is returned.
 * If false is returned, the value is undefined and should be
 * discarded.
 */
bool SensorInput::frequency_value(double* value) {
    
    // Poll value from sensor
    uint16_t rawValue = 0;
    distance_us_get_distance_value(&distanceFrequency, &rawValue);
    
    if (rawValue <= cfg->i(SENSOR_FREQ_MAX_VALUE)) {
        // normalize the value to [0,1]
        *value = ((double) rawValue - cfg->i(SENSOR_FREQ_MIN_VALUE)) 
                / (cfg->i(SENSOR_FREQ_MAX_VALUE) - cfg->i(SENSOR_FREQ_MIN_VALUE));
        return true;
    } else {
        // do not report values over the threshold
        return false;
    }
}

/*
 * Writes the current volume value into the given double, 
 * normalized to [0,1], if true is returned.
 * If false is returned, the value is undefined and should be
 * discarded.
 */
bool SensorInput::volume_value(double* value) {
    
    // Poll value from sensor
    uint16_t rawValue = 0;
    distance_us_get_distance_value(&distanceVolume, &rawValue);
    
    // cap values at the threshold
    if (rawValue >= cfg->i(SENSOR_VOL_MAX_VALUE)) {
        rawValue = cfg->i(SENSOR_VOL_MAX_VALUE);
    }
    
    // normalize the value to [0,1]
    *value = (1 - ((double) rawValue - cfg->i(SENSOR_VOL_MIN_VALUE)) 
                / (cfg->i(SENSOR_VOL_MAX_VALUE) - cfg->i(SENSOR_VOL_MIN_VALUE)));
    return true;
}

/*
 * clean up on exit
 */
void SensorInput::finish() {
    
    distance_us_destroy(&distanceFrequency);
    distance_us_destroy(&distanceVolume);
    ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
