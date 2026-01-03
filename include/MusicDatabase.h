#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>

namespace Chunithm {

struct MusicInfo {
    int32_t id = 0;
    std::string name;
    std::string sortName;
    std::string artistName;
    std::string genre;
    std::string jacketFile;
    std::wstring jacketFullPath;
};

class MusicDatabase {
public:
    static bool Initialize();

    static std::string GetMusicName(int32_t musicId);

    static const MusicInfo* GetMusicInfo(int32_t musicId);

    static size_t GetMusicCount();

    static bool Reload();

    static std::wstring GetJacketPath(int32_t musicId);

    static std::string GetJacketFileName(int32_t musicId);

private:
    static void ScanDirectory(const std::wstring& path, int depth = 0);

    static bool ParseMusicXml(const std::wstring& filePath);

    static std::string ExtractNestedPath(const std::string& xml, const std::string& parentTag);

    static std::string ExtractTagValue(const std::string& xml, const std::string& tag);

    static int32_t ExtractNestedId(const std::string& xml, const std::string& parentTag);

    static std::string ExtractNestedStr(const std::string& xml, const std::string& parentTag);

    static std::unordered_map<int32_t, MusicInfo> s_musicData;
    static bool s_initialized;
};

}
