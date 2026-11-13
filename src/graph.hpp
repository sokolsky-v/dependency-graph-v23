#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using Graph = std::unordered_map<std::string, std::vector<std::string>>;

Graph buildDependencyGraph(
    const std::string& repositoryUrl,
    const std::string& rootPackage,
    const std::string& filterSubstring,
    bool testMode
);