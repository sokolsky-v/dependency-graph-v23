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
        DependencyGraph forwardGraph;
        if (config.testMode) {
            forwardGraph = loadTestRepositoryGraph(config.repositoryUrl, config.filterSubstring);
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

        std::cout << "\n--- Этап 5: Визуализация (D2 & ASCII Tree) ---\n";
        if (config.testMode && !forwardGraph.empty()) {
            saveD2Diagram(forwardGraph, config.outputFile);
            std::cout << "Граф сохранен в файл: " << config.outputFile << " (D2 формат)\n";

            if (config.asciiTree) {
                std::cout << "ASCII-дерево зависимостей для пакета " << config.packageName << ":\n";
                std::unordered_set<std::string> treeVisited;
                printAsciiTree(config.packageName, forwardGraph, treeVisited);
            }
        } else {
            std::cout << "Визуализация в тестовом режиме использует файл графа.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}