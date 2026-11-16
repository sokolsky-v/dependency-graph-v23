#ifndef GRAPH_HPP
#define GRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using DependencyGraph = std::unordered_map<std::string, std::vector<std::string>>;

DependencyGraph loadTestRepositoryGraph(const std::string& filePath, const std::string& filterSubstring);
DependencyGraph buildReverseGraph(const DependencyGraph& forwardGraph);
void findReverseDependenciesDfs(
    const std::string& currentPackage,
    const DependencyGraph& reverseGraph,
    std::unordered_set<std::string>& visited,
    std::unordered_set<std::string>& visiting,
    std::vector<std::string>& result
);

#endif
