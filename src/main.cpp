#include "config.hpp"
#include "maven.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: app <config.json>\n";
        return 1;
    }
    try {
        Config config = loadConfig(argv[1]);
        
        std::cout << "--- Config loaded successfully ---\n";
        printConfig(config);
        
        std::cout << "\n--- Этап 2: Прямые зависимости пакета " << config.packageName 
                  << " (version: " << config.packageVersion << ") ---\n";
                  
        auto dependencies = getDirectDependencies(
            config.repositoryUrl, 
            config.packageName, 
            config.packageVersion, 
            config.testMode
        );

        if (dependencies.empty()) {
            std::cout << "Прямые зависимости не найдены или список пуст.\n";
        } else {
            for (const auto& dep : dependencies) {
                std::cout << " - Group: " << dep.groupId 
                          << ", Artifact: " << dep.artifactId 
                          << ", Version: " << dep.version << "\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}