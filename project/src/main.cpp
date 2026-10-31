#include "config.hpp"

#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: app <config.json>\n";
        return 1;
    }
    try {
        Config config = loadConfig(argv[1]);
        printConfig(config);
    } catch (const std::exception& e) {
        std::cerr << "Error: "
                  << e.what() << "\n";
        return 1;
    }
    return 0;
}