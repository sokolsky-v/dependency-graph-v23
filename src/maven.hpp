#pragma once
#include <string>
#include <vector>

struct Dependency
{
    std::string groupId;
    std::string artifactId;
    std::string version;
};

std::vector<Dependency> getDirectDependencies(
    const std::string& repositoryUrl,
    const std::string& packageName,
    const std::string& packageVersion,
    bool testMode
);