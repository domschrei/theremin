#ifndef THEREMIN_CONFIGURATION_H
#define THEREMIN_CONFIGURATION_H

#include <iostream>
#include <libconfig.h++>

using namespace libconfig;

class Configuration {

private:
    Config config;
    
public:
    void load();
    
    bool b(const char* value);
    int i(const char* value);
    double d(const char* value);
    std::string str(const char* value);
};

#endif
