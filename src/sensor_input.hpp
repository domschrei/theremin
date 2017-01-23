#ifndef THEREMIN_SENSOR_INPUT_H
#define THEREMIN_SENSOR_INPUT_H

#include "ip_connection.h"
#include "bricklet_distance_us.h"

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
