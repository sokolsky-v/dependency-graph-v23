#include "config.hpp"
#include "maven.hpp"
#include "graph.hpp"
#include <iostream>
#include <unordered_set>
#include <vector>

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

        std::cout << "\n--- Этап 4: Обратные зависимости (Reverse Dependencies) ---\n";
        if (config.testMode) {
            DependencyGraph forwardGraph = loadTestRepositoryGraph(config.repositoryUrl, config.filterSubstring);
            DependencyGraph reverseGraph = buildReverseGraph(forwardGraph);

            std::unordered_set<std::string> visited;
            std::unordered_set<std::string> visiting;
            std::vector<std::string> reverseDeps;

            findReverseDependenciesDfs(config.packageName, reverseGraph, visited, visiting, reverseDeps);

            if (reverseDeps.empty()) {
                std::cout << "Обратные зависимости не найдены.\n";
            } else {
                for (const auto& pkg : reverseDeps) {
                    std::cout << " - Dependent: " << pkg << "\n";
                }
            }
        } else {
            std::cout << "Режим обратных зависимостей для боевого репозитория поддерживается в тестовом графе.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}