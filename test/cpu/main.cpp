#include "Harness.hpp"
#include <boost/json.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>
#include <print>
#include <fstream>
#include <sstream>
#include <filesystem>

int main() {
    std::filesystem::path sstPath{"test/cpu/sst"};
    for (auto&& fileName : std::filesystem::directory_iterator(sstPath)) {
        if (fileName.path().filename() == "10.json") continue;  // skip STOP instruction tests
        std::ifstream testFile{fileName.path()};
        std::stringstream ss;
        ss << testFile.rdbuf();
        auto testConfigs = boost::json::parse(ss.str());
        Harness harness;
        for (auto&& testConfig : testConfigs.as_array()) {
            harness.test(testConfig);
        }
    }
}