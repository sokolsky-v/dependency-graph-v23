#include "../src/config.hpp"

#include <iostream>
#include <cassert>
#include <fstream>

namespace {

void writeTestFile(const std::string& path,
                   const std::string& content) {
    std::ofstream file(path);
    file << content;
}

void testValidConfig() {
    const std::string path = "test_valid.json";
    writeTestFile(path,
        "{"
        "\"package_name\": \"test\","
        "\"repository_url\": "
        "\"https://example.com\","
        "\"test_mode\": false,"
        "\"package_version\": \"1.0\","
        "\"output_file\": \"out.png\","
        "\"ascii_tree\": true,"
        "\"filter_substring\": \"\""
        "}");
    Config config = loadConfig(path);
    assert(config.packageName == "test");
    assert(config.packageVersion == "1.0");
    assert(config.testMode == false);
    assert(config.asciiTree == true);
    std::cout << "testValidConfig: PASSED\n";
}

void testMissingField() {
    const std::string path = "test_missing.json";
    writeTestFile(path,
        "{\"package_name\": \"test\"}");
    bool caught = false;
    try {
        loadConfig(path);
    } catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
    std::cout << "testMissingField: PASSED\n";
}

void testInvalidBool() {
    const std::string path = "test_bool.json";
    writeTestFile(path,
        "{"
        "\"package_name\": \"test\","
        "\"repository_url\": "
        "\"https://example.com\","
        "\"test_mode\": \"yes\","
        "\"package_version\": \"1.0\","
        "\"output_file\": \"out.png\","
        "\"ascii_tree\": true,"
        "\"filter_substring\": \"\""
        "}");
    bool caught = false;
    try {
        loadConfig(path);
    } catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
    std::cout << "testInvalidBool: PASSED\n";
}

void testEmptyPackageName() {
    const std::string path = "test_empty.json";
    writeTestFile(path,
        "{"
        "\"package_name\": \"\","
        "\"repository_url\": "
        "\"https://example.com\","
        "\"test_mode\": false,"
        "\"package_version\": \"1.0\","
        "\"output_file\": \"out.png\","
        "\"ascii_tree\": true,"
        "\"filter_substring\": \"\""
        "}");
    bool caught = false;
    try {
        loadConfig(path);
    } catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
    std::cout
        << "testEmptyPackageName: PASSED\n";
}

void testInvalidUrl() {
    const std::string path = "test_url.json";
    writeTestFile(path,
        "{"
        "\"package_name\": \"test\","
        "\"repository_url\": \"not-a-url\","
        "\"test_mode\": false,"
        "\"package_version\": \"1.0\","
        "\"output_file\": \"out.png\","
        "\"ascii_tree\": true,"
        "\"filter_substring\": \"\""
        "}");
    bool caught = false;
    try {
        loadConfig(path);
    } catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
    std::cout << "testInvalidUrl: PASSED\n";
}

void testInvalidExtension() {
    const std::string path = "test_ext.json";
    writeTestFile(path,
        "{"
        "\"package_name\": \"test\","
        "\"repository_url\": "
        "\"https://example.com\","
        "\"test_mode\": false,"
        "\"package_version\": \"1.0\","
        "\"output_file\": \"out.txt\","
        "\"ascii_tree\": true,"
        "\"filter_substring\": \"\""
        "}");
    bool caught = false;
    try {
        loadConfig(path);
    } catch (const std::exception&) {
        caught = true;
    }
    assert(caught);
    std::cout
        << "testInvalidExtension: PASSED\n";
}

}

int main() {
    testValidConfig();
    testMissingField();
    testInvalidBool();
    testEmptyPackageName();
    testInvalidUrl();
    testInvalidExtension();
    std::cout << "All tests passed!\n";
    return 0;
}