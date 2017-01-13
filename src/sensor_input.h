#ifndef THEREMIN_SENSOR_INPUT_H
#define THEREMIN_SENSOR_INPUT_H

#include "../tinkerforge/source/ip_connection.h"
#include "../tinkerforge/source/bricklet_distance_us.h"

class SensorInput {

public:
    void setup_sensors();
    void finish();
    
    DistanceUS distanceFrequency;
    DistanceUS distanceVolume;
    
private:
    IPConnection ipcon;
    
};

#endif
