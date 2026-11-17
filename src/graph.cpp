#include "graph.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cstdlib>

static std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

static bool isFiltered(const std::string& pkg, const std::string& filterSubstring) {
    return !filterSubstring.empty() && pkg.find(filterSubstring) != std::string::npos;
}

DependencyGraph loadTestRepositoryGraph(const std::string& filePath, const std::string& filterSubstring) {
    DependencyGraph graph;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return graph;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }

        size_t sepPos = line.find(':');
        size_t sepLen = 1;
        if (sepPos == std::string::npos) {
            sepPos = line.find("->");
            sepLen = 2;
        }

        std::string pkg;
        std::string depsPart;
        if (sepPos != std::string::npos) {
            pkg = trim(line.substr(0, sepPos));
            depsPart = trim(line.substr(sepPos + sepLen));
        } else {
            pkg = line;
        }

        if (pkg.empty() || isFiltered(pkg, filterSubstring)) {
            continue;
        }

        if (graph.find(pkg) == graph.end()) {
            graph[pkg] = {};
        }

        std::istringstream iss(depsPart);
        std::string dep;
        while (iss >> dep) {
            if (!isFiltered(dep, filterSubstring)) {
                graph[pkg].push_back(dep);
                if (graph.find(dep) == graph.end()) {
                    graph[dep] = {};
                }
            }
        }
    }
    return graph;
}

DependencyGraph buildReverseGraph(const DependencyGraph& forwardGraph) {
    DependencyGraph reverseGraph;
    for (const auto& pair : forwardGraph) {
        if (reverseGraph.find(pair.first) == reverseGraph.end()) {
            reverseGraph[pair.first] = {};
        }
        for (const auto& dep : pair.second) {
            reverseGraph[dep].push_back(pair.first);
        }
    }
    return reverseGraph;
}

void findReverseDependenciesDfs(
    const std::string& currentPackage,
    const DependencyGraph& reverseGraph,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& visiting,
    std::vector<std::string>& result
) {
    if (visiting.count(currentPackage) || visited.count(currentPackage)) {
        return;
    }

    visiting.insert(currentPackage);

    auto it = reverseGraph.find(currentPackage);
    if (it != reverseGraph.end()) {
        for (const auto& dependent : it->second) {
            if (!visiting.count(dependent) && !visited.count(dependent)) {
                result.push_back(dependent);
                findReverseDependenciesDfs(dependent, reverseGraph, visited, visiting, result);
            }
        }
    }

    visiting.erase(currentPackage);
    visited.insert(currentPackage);
}

void printAsciiTree(
    const std::string& currentPackage,
    const DependencyGraph& graph,
    std::unordered_set<std::string>& visited,
    int depth
) {
    if (visited.count(currentPackage)) {
        for (int i = 0; i < depth; ++i) {
            std::cout << "  ";
        }
        std::cout << "- " << currentPackage << " (cyclic)\n";
        return;
    }

    for (int i = 0; i < depth; ++i) {
        std::cout << "  ";
    }
    std::cout << "- " << currentPackage << "\n";

    visited.insert(currentPackage);

    auto it = graph.find(currentPackage);
    if (it != graph.end()) {
        for (const auto& dep : it->second) {
            printAsciiTree(dep, graph, visited, depth + 1);
        }
    }

    visited.erase(currentPackage);
}

void saveD2Diagram(const DependencyGraph& graph, const std::string& outputPath) {
    std::ofstream d2File("graph.d2");
    if (!d2File.is_open()) {
        return;
    }

    for (const auto& pair : graph) {
        for (const auto& dep : pair.second) {
            d2File << pair.first << " -> " << dep << "\n";
        }
    }
    d2File.close();

    std::string command = "d2 graph.d2 " + outputPath;
    std::system(command.c_str());
}