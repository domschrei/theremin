#include <stdio.h>
#include <functional>

#include "config.h"
#include "sensor_input.h"

#define HOST "localhost"
#define PORT 4223
#define UID_FREQUENCY "zmj" // UID of ultrasonic distance sensor #1
#define UID_VOLUME "zn8" // UID of ultrasonic distance sensor #2

/*
 * Connects to the sensor brick at localhost:4223 and to the
 * two expected distance sensors.
 */
void SensorInput::setup_sensors() {
    
    // Create IP connection
    ipcon_create(&ipcon);

    // Create device object
    distance_us_create(&distanceFrequency, UID_FREQUENCY, &ipcon);
    distance_us_create(&distanceVolume, UID_VOLUME, &ipcon);

    // Connect to brickd
    if(ipcon_connect(&ipcon, HOST, PORT) < 0) {
        fprintf(stderr, "Could not connect to the sensors.\nPlease make sure that the Brick daemon is running (`sudo brickd --daemon`) and that the master brick is connected to the PC.\n");
        exit(1);
    }

    // Turn off moving average (because it causes higher latency)
    distance_us_set_moving_average(&distanceFrequency, 0);
    distance_us_set_moving_average(&distanceVolume, 0);
}

/*
 * clean up on exit
 */
void SensorInput::finish() {
    
    distance_us_destroy(&distanceFrequency);
    distance_us_destroy(&distanceVolume);
    ipcon_destroy(&ipcon); // Calls ipcon_disconnect internally
}
