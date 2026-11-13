#include "config.hpp"
#include "maven.hpp"
#include "graph.hpp"
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
        
        std::cout << "\n--- Этап 3: Построение графа зависимостей (DFS с рекурсией) ---\n";
                  
        Graph graph = buildDependencyGraph(
            config.repositoryUrl,
            config.packageName,
            config.filterSubstring,
            config.testMode
        );

        if (graph.empty()) {
            std::cout << "Граф зависимостей пуст.\n";
        } else {
            std::cout << "Построенный граф зависимостей (adjacency list):\n";
            for (const auto& pair : graph) {
                std::cout << "Пакет: " << pair.first << " -> Зависимости: ";
                for (const auto& dep : pair.second) {
                    std::cout << dep << " ";
                }
                std::cout << "\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}