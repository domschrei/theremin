#include <iostream>
#include <libconfig.h++>

#include "configuration.hpp"

void Configuration::load() {
    
    try {
        config.readFile("theremin.cfg");
    } catch (const FileIOException &fioex) {
        std::cerr << "I/O error while reading file." << std::endl;
        exit(1);
    } catch (const ParseException &pex) {
        std::cerr << "Parse error at " << pex.getFile() << ":" << pex.getLine()
                << " - " << pex.getError() << std::endl;
        exit(1);
    }
}

bool Configuration::b(const char* value) {
    
    bool result;
    if (config.getRoot().lookupValue(value, result)) {
        return result;
    } else {
        std::cerr << "Error: Illegal configuration query \"" 
            << value << "\"." << std::endl;
        exit(1);
    }       
}

int Configuration::i(const char* value) {
    
    int result;
    if (config.getRoot().lookupValue(value, result)) {
        return result;
    } else {
        std::cerr << "Error: Illegal configuration query \"" 
            << value << "\"." << std::endl;
        exit(1);
    }
}

double Configuration::d(const char* value) {
    
    double result;
    if (config.getRoot().lookupValue(value, result)) {
        return result;
    } else {
        std::cerr << "Error: Illegal configuration query \"" 
            << value << "\"." << std::endl;
        exit(1);
    }
}

std::string Configuration::str(const char* value) {
    
    std::string result;
    if (config.getRoot().lookupValue(value, result)) {
        return result;
    } else {
        std::cerr << "Error: Illegal configuration query \"" 
            << value << "\"." << std::endl;
        exit(1);
    }   
}
