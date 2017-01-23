#ifndef THEREMIN_SENSOR_INPUT_H
#define THEREMIN_SENSOR_INPUT_H

#include "../tinkerforge/source/ip_connection.h"
#include "../tinkerforge/source/bricklet_distance_us.h"

#include "configuration.hpp"

class SensorInput {

public:
    void setup_sensors(Configuration* cfg);
    void finish();
    bool frequency_value(double* value);
    bool volume_value(double* value);
    
    DistanceUS distanceFrequency;
    DistanceUS distanceVolume;
    
private:
    Configuration* cfg;
    IPConnection ipcon;
    
};

#endif
