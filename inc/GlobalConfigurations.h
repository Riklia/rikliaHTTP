#pragma once

#include <string>
#include <iostream>
#include <unordered_map>

class GlobalConfigurations {
public:
    static GlobalConfigurations* instance();
    void setConfigurationDir(std::string val) {
        configurationDir = val;
        initMipeMap();
    }
    std::string getConfigurationDir() { return configurationDir; }
    std::string guessMimeType(std::string);

private:
    GlobalConfigurations();
    GlobalConfigurations(const GlobalConfigurations&) = delete;
    GlobalConfigurations& operator=(const GlobalConfigurations&) = delete;
    void initMipeMap();

    static GlobalConfigurations* _instance;
    std::string configurationDir;
    std::unordered_map<std::string, std::string> mimeMap;
};
