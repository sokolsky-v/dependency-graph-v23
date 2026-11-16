#include "config.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace {

bool isValidUrl(const std::string& url) {
    if (url.rfind("http://", 0) == 0 || url.rfind("https://", 0) == 0) {
        return true;
    }
    if (url.find("repo1.maven.org") != std::string::npos || url.find("maven.org") != std::string::npos) {
        return true;
    }
    return false;
}

}

Config loadConfig(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open config file: " + filepath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    Config config;

    auto parseStringField = [&](const std::string& fieldName, std::string& dest) {
        size_t keyPos = content.find("\"" + fieldName + "\"");
        if (keyPos == std::string::npos) return;
        size_t colonPos = content.find(':', keyPos);
        if (colonPos == std::string::npos) return;
        size_t startQuote = content.find('"', colonPos);
        if (startQuote == std::string::npos) return;
        size_t endQuote = content.find('"', startQuote + 1);
        if (endQuote == std::string::npos) return;
        dest = content.substr(startQuote + 1, endQuote - startQuote - 1);
    };

    auto parseBoolField = [&](const std::string& fieldName, bool& dest) {
        size_t keyPos = content.find("\"" + fieldName + "\"");
        if (keyPos == std::string::npos) return;
        size_t colonPos = content.find(':', keyPos);
        if (colonPos == std::string::npos) return;
        size_t valStart = content.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valStart == std::string::npos) return;
        if (content.rfind("true", valStart) == valStart) {
            dest = true;
        } else if (content.rfind("false", valStart) == valStart) {
            dest = false;
        }
    };

    parseStringField("package_name", config.packageName);
    parseStringField("repository_url", config.repositoryUrl);
    parseBoolField("test_mode", config.testMode);
    parseStringField("package_version", config.packageVersion);
    parseStringField("output_file", config.outputFile);
    parseBoolField("ascii_tree", config.asciiTree);
    parseStringField("filter_substring", config.filterSubstring);

    if (config.packageName.empty()) {
        throw std::runtime_error("Config error: package_name is missing.");
    }
    if (config.repositoryUrl.empty()) {
        throw std::runtime_error("Config error: repository_url is missing.");
    }
    if (!config.testMode && !isValidUrl(config.repositoryUrl)) {
        throw std::runtime_error("Error: Invalid URL: " + config.repositoryUrl);
    }

    return config;
}

void printConfig(const Config& config) {
    std::cout << "package_name = " << config.packageName << "\n";
    std::cout << "repository_url = " << config.repositoryUrl << "\n";
    std::cout << "test_mode = " << (config.testMode ? "true" : "false") << "\n";
    std::cout << "package_version = " << config.packageVersion << "\n";
    std::cout << "output_file = " << config.outputFile << "\n";
    std::cout << "ascii_tree = " << (config.asciiTree ? "true" : "false") << "\n";
    std::cout << "filter_substring = " << config.filterSubstring << "\n";
}