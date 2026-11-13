#include "maven.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <algorithm>

namespace
{

    std::string extractTagContent(const std::string &xml, const std::string &tag, size_t &pos)
    {
        std::string openTag = "<" + tag + ">";
        std::string closeTag = "</" + tag + ">";

        size_t start = xml.find(openTag, pos);
        if (start == std::string::npos)
            return "";
        start += openTag.length();

        size_t end = xml.find(closeTag, start);
        if (end == std::string::npos)
            return "";

        pos = end + closeTag.length();
        return xml.substr(start, end - start);
    }

    std::vector<Dependency> parsePomContent(const std::string &content)
    {
        std::vector<Dependency> dependencies;
        size_t pos = 0;

        while (true)
        {
            size_t depStart = content.find("<dependency>", pos);
            if (depStart == std::string::npos)
                break;

            pos = depStart + 12;
            size_t depEnd = content.find("</dependency>", pos);
            if (depEnd == std::string::npos)
                break;

            std::string depBlock = content.substr(pos, depEnd - pos);
            size_t innerPos = 0;

            std::string gId = extractTagContent(depBlock, "groupId", innerPos);
            innerPos = 0;
            std::string aId = extractTagContent(depBlock, "artifactId", innerPos);
            innerPos = 0;
            std::string ver = extractTagContent(depBlock, "version", innerPos);

            if (!aId.empty())
            {
                dependencies.push_back({gId, aId, ver});
            }
            pos = depEnd + 13;
        }

        return dependencies;
    }

}

std::vector<Dependency> getDirectDependencies(
    const std::string &repositoryUrl,
    const std::string & /*packageName*/,
    const std::string & /*packageVersion*/,
    bool testMode)
{
    std::string pomContent;

    if (testMode)
    {
        if (!std::filesystem::exists(repositoryUrl))
        {
            throw std::runtime_error("Test repository file not found: " + repositoryUrl);
        }
        std::ifstream file(repositoryUrl);
        std::stringstream buffer;
        buffer << file.rdbuf();
        pomContent = buffer.str();
    }
    else
    {
        if (std::filesystem::exists(repositoryUrl))
        {
            std::ifstream file(repositoryUrl);
            std::stringstream buffer;
            buffer << file.rdbuf();
            pomContent = buffer.str();
        }
        else
        {
            throw std::runtime_error(
                "Network fetching without external libraries "
                "requires local pom.xml or test_mode=true in this prototype.");
        }
    }

    return parsePomContent(pomContent);
}