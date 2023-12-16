#include "GlobalConfigurations.h"
#include "utils/file_utils.h"

GlobalConfigurations* GlobalConfigurations::_instance = nullptr;

GlobalConfigurations* GlobalConfigurations::instance() {
    if (!_instance) {
        _instance = new GlobalConfigurations();
    }
    return _instance;
}

GlobalConfigurations::GlobalConfigurations() {
    configurationDir = file_utils::getCurrentDirectory();
}

void GlobalConfigurations::initMipeMap() {
    mimeMap = file_utils::parseFile(configurationDir + "/mime.types", true);
}

std::string GlobalConfigurations::guessMimeType(std::string filename) {
    std::string extension = file_utils::getExtension(filename);
    return mimeMap[extension];
}