#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Chunithm {

struct JacketEntry {
    std::string title;
    std::string imageUrl;
};

class JacketProvider {
public:
    static void LoadJacketData();

    static bool LoadJacketDataFromFile(const std::string& filePath);

    static std::string GetJacketUrl(int32_t musicId);

    static bool IsConfigured();

    static size_t GetJacketCount();

private:
    static std::unordered_map<int32_t, JacketEntry> s_jacketData;
    static bool ParseJsonData(const std::string& jsonData);
};

}
