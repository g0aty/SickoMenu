#pragma once
#include <fstream>
#include <string>

namespace ConfigHelper {
    inline const std::string fileName = "SickoMenu_Config.ini";

    inline void Save() {
        std::ofstream file(fileName);
        if (file.is_open()) {
            file.close();
        }
    }

    inline void Load() {
        std::ifstream file(fileName);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            size_t delimiter = line.find('=');
            if (delimiter == std::string::npos) continue;

            std::string key = line.substr(0, delimiter);
            std::string value = line.substr(delimiter + 1);
        }
        file.close();
    }
}
