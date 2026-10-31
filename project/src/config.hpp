#pragma once
#include <string>

using namespace std;

struct Config
{
    string packageName;
    string repositoryUrl;
    bool testMode;
    string packageVersion;
    string outputFile;
    bool asciiTree;
    string filterSubstring;
};

Config loadConfig(const string& path);
void printConfig(const Config& config);