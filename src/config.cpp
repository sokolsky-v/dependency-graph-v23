#include "config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <filesystem>
#include <iostream>

namespace
{

    const std::string kHttpPrefix = "http://";
    const std::string kHttpsPrefix = "https://";
    const std::string kPngExt = ".png";
    const std::string kSvgExt = ".svg";

    std::string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \t\r\n\"");
        if (start == std::string::npos)
        {
            return "";
        }
        size_t end = str.find_last_not_of(" \t\r\n\",");
        return str.substr(start, end - start + 1);
    }

    std::string readFile(const std::string &path)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            throw std::runtime_error(
                "Cannot open config file: " + path);
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::string findValue(const std::string &content,
                          const std::string &key)
    {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = content.find(searchKey);
        if (pos == std::string::npos)
        {
            throw std::runtime_error(
                "Missing field: " + key);
        }
        pos = content.find(':',
                           pos + searchKey.length());
        if (pos == std::string::npos)
        {
            throw std::runtime_error(
                "Invalid format for: " + key);
        }
        size_t valueStart = content.find_first_not_of(
            " \t\r\n", pos + 1);
        if (valueStart == std::string::npos)
        {
            throw std::runtime_error(
                "Empty value for: " + key);
        }
        if (content[valueStart] == '"')
        {
            size_t valueEnd = content.find('"',
                                           valueStart + 1);
            if (valueEnd == std::string::npos)
            {
                throw std::runtime_error(
                    "Unterminated string: " + key);
            }
            return content.substr(
                valueStart + 1,
                valueEnd - valueStart - 1);
        }
        size_t valueEnd = content.find_first_of(
            ",}\r\n", valueStart);
        if (valueEnd == std::string::npos)
        {
            valueEnd = content.length();
        }
        return trim(content.substr(
            valueStart, valueEnd - valueStart));
    }

    bool parseBool(const std::string &value,
                   const std::string &fieldName)
    {
        if (value == "true")
        {
            return true;
        }
        if (value == "false")
        {
            return false;
        }
        throw std::runtime_error(
            "Invalid bool for " + fieldName + ": " + value);
    }

    void validateString(const std::string &value,
                        const std::string &fieldName)
    {
        if (value.empty())
        {
            throw std::runtime_error(
                fieldName + " cannot be empty");
        }
    }

    void validateUrl(const std::string &url,
                     bool testMode)
    {
        if (testMode)
        {
            if (!std::filesystem::exists(url))
            {
                throw std::runtime_error(
                    "Test repository file not found: " + url);
            }
        }
        else
        {
            bool hasHttp = url.find(kHttpPrefix) == 0;
            bool hasHttps = url.find(kHttpsPrefix) == 0;
            if (!hasHttp && !hasHttps)
            {
                throw std::runtime_error(
                    "Invalid URL: " + url);
            }
        }
    }

    void validateOutputFile(const std::string &file)
    {
        size_t dotPos = file.rfind('.');
        if (dotPos == std::string::npos)
        {
            throw std::runtime_error(
                "Output file must have .png "
                "or .svg extension");
        }
        std::string ext = file.substr(dotPos);
        if (ext != kPngExt && ext != kSvgExt)
        {
            throw std::runtime_error(
                "Output file must have .png "
                "or .svg extension");
        }
    }

}

Config loadConfig(const std::string &path)
{
    Config config;
    std::string content = readFile(path);

    config.packageName = findValue(
        content, "package_name");
    config.repositoryUrl = findValue(
        content, "repository_url");
    std::string testModeStr = findValue(
        content, "test_mode");
    config.testMode = parseBool(
        testModeStr, "test_mode");
    config.packageVersion = findValue(
        content, "package_version");
    config.outputFile = findValue(
        content, "output_file");
    std::string asciiStr = findValue(
        content, "ascii_tree");
    config.asciiTree = parseBool(
        asciiStr, "ascii_tree");
    config.filterSubstring = findValue(
        content, "filter_substring");

    validateString(config.packageName,
                   "package_name");
    validateString(config.packageVersion,
                   "package_version");
    validateString(config.outputFile,
                   "output_file");
    validateUrl(config.repositoryUrl,
                config.testMode);
    validateOutputFile(config.outputFile);

    return config;
}

void printConfig(const Config &config)
{
    std::cout << "package_name = "
              << config.packageName << "\n";
    std::cout << "repository_url = "
              << config.repositoryUrl << "\n";
    std::cout << "test_mode = "
              << (config.testMode
                      ? "true"
                      : "false")
              << "\n";
    std::cout << "package_version = "
              << config.packageVersion << "\n";
    std::cout << "output_file = "
              << config.outputFile << "\n";
    std::cout << "ascii_tree = "
              << (config.asciiTree
                      ? "true"
                      : "false")
              << "\n";
    std::cout << "filter_substring = "
              << config.filterSubstring
              << "\n";
}