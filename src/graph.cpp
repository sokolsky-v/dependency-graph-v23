#include "graph.hpp"
#include "maven.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace {

void dfs(
    const std::string& currentPkg,
    const std::string& repoUrl,
    const std::string& filterSub,
    bool testMode,
    Graph& graph,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& visiting
) {
    if (visiting.count(currentPkg)) {
        return;
    }
    if (visited.count(currentPkg)) {
        return;
    }

    visiting.insert(currentPkg);

    auto directDeps = getDirectDependencies(repoUrl, currentPkg, "", testMode);

    std::vector<std::string> validDeps;
    for (const auto& dep : directDeps) {
        std::string depName = dep.artifactId;

        if (!filterSub.empty() && depName.find(filterSub) != std::string::npos) {
            continue;
        }

        validDeps.push_back(depName);
        dfs(depName, repoUrl, filterSub, testMode, graph, visited, visiting);
    }

    graph[currentPkg] = validDeps;

    visiting.erase(currentPkg);
    visited.insert(currentPkg);
}

}

Graph buildDependencyGraph(
    const std::string& repositoryUrl,
    const std::string& rootPackage,
    const std::string& filterSubstring,
    bool testMode
) {
    Graph graph;
    std::unordered_set<std::string> visited;
    std::unordered_set<std::string> visiting;

    dfs(rootPackage, repositoryUrl, filterSubstring, testMode, graph, visited, visiting);

    return graph;
}