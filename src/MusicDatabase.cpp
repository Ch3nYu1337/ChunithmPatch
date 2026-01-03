#include "MusicDatabase.h"
#include <Windows.h>
#include <fstream>
#include <sstream>

namespace Chunithm {

	std::unordered_map<int32_t, MusicInfo> MusicDatabase::s_musicData;
	bool MusicDatabase::s_initialized = false;

	bool MusicDatabase::Initialize() {
		if (s_initialized) {
			return true;
		}

		s_musicData.clear();

		wchar_t currentDir[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, currentDir);

		ScanDirectory(currentDir, 0);

		std::wstring parentDir = currentDir;
		size_t lastSlash = parentDir.find_last_of(L"\\/");
		if (lastSlash != std::wstring::npos) {
			parentDir = parentDir.substr(0, lastSlash);
			ScanDirectory(parentDir, 0);
		}

		s_initialized = true;
		return !s_musicData.empty();
	}

	std::string MusicDatabase::GetMusicName(int32_t musicId) {
		auto it = s_musicData.find(musicId);
		if (it != s_musicData.end()) {
			return it->second.name;
		}
		return "";
	}

	const MusicInfo* MusicDatabase::GetMusicInfo(int32_t musicId) {
		auto it = s_musicData.find(musicId);
		if (it != s_musicData.end()) {
			return &it->second;
		}
		return nullptr;
	}

	size_t MusicDatabase::GetMusicCount() {
		return s_musicData.size();
	}

	bool MusicDatabase::Reload() {
		s_initialized = false;
		return Initialize();
	}

	std::wstring MusicDatabase::GetJacketPath(int32_t musicId) {
		auto it = s_musicData.find(musicId);
		if (it != s_musicData.end()) {
			return it->second.jacketFullPath;
		}
		return L"";
	}

	std::string MusicDatabase::GetJacketFileName(int32_t musicId) {
		auto it = s_musicData.find(musicId);
		if (it != s_musicData.end()) {
			return it->second.jacketFile;
		}
		return "";
	}

	void MusicDatabase::ScanDirectory(const std::wstring& path, int depth) {
		if (depth > 10) {
			return;
		}

		std::wstring searchPath = path + L"\\*";
		WIN32_FIND_DATAW findData;
		HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findData);

		if (hFind == INVALID_HANDLE_VALUE) {
			return;
		}

		do {
			std::wstring fileName = findData.cFileName;

			if (fileName == L"." || fileName == L"..") {
				continue;
			}

			std::wstring fullPath = path + L"\\" + fileName;

			if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				ScanDirectory(fullPath, depth + 1);
			}
			else {
				if (fileName == L"Music.xml" || fileName == L"music.xml") {
					ParseMusicXml(fullPath);
				}
			}
		} while (FindNextFileW(hFind, &findData));

		FindClose(hFind);
	}

	bool MusicDatabase::ParseMusicXml(const std::wstring& filePath) {
		std::ifstream file(filePath, std::ios::binary);
		if (!file.is_open()) {
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string xml = buffer.str();
		file.close();

		int32_t musicId = ExtractNestedId(xml, "name");
		if (musicId <= 0) {
			return false;
		}

		if (s_musicData.find(musicId) != s_musicData.end()) {
			return true;
		}

		MusicInfo info;
		info.id = musicId;

		info.name = ExtractNestedStr(xml, "name");

		info.sortName = ExtractTagValue(xml, "sortName");

		info.artistName = ExtractNestedStr(xml, "artistName");

		info.genre = ExtractNestedStr(xml, "genreNames");

		info.jacketFile = ExtractNestedPath(xml, "jaketFile");

		if (!info.jacketFile.empty()) {
			std::wstring musicXmlDir = filePath;
			size_t lastSlash = musicXmlDir.find_last_of(L"\\/");
			if (lastSlash != std::wstring::npos) {
				musicXmlDir = musicXmlDir.substr(0, lastSlash);
			}
			std::wstring jacketFileW(info.jacketFile.begin(), info.jacketFile.end());
			info.jacketFullPath = musicXmlDir + L"\\" + jacketFileW;
		}

		if (!info.name.empty()) {
			s_musicData[musicId] = info;
			return true;
		}

		return false;
	}

	std::string MusicDatabase::ExtractTagValue(const std::string& xml, const std::string& tag) {
		std::string openTag = "<" + tag + ">";
		std::string closeTag = "</" + tag + ">";

		size_t start = xml.find(openTag);
		if (start == std::string::npos) {
			return "";
		}
		start += openTag.length();

		size_t end = xml.find(closeTag, start);
		if (end == std::string::npos) {
			return "";
		}

		return xml.substr(start, end - start);
	}

	int32_t MusicDatabase::ExtractNestedId(const std::string& xml, const std::string& parentTag) {
		std::string openTag = "<" + parentTag + ">";
		std::string closeTag = "</" + parentTag + ">";

		size_t parentStart = xml.find(openTag);
		if (parentStart == std::string::npos) {
			return -1;
		}
		parentStart += openTag.length();

		size_t parentEnd = xml.find(closeTag, parentStart);
		if (parentEnd == std::string::npos) {
			return -1;
		}

		std::string parentContent = xml.substr(parentStart, parentEnd - parentStart);

		std::string idValue = ExtractTagValue(parentContent, "id");
		if (idValue.empty()) {
			return -1;
		}

		try {
			return std::stoi(idValue);
		}
		catch (...) {
			return -1;
		}
	}

	std::string MusicDatabase::ExtractNestedStr(const std::string& xml, const std::string& parentTag) {
		std::string openTag = "<" + parentTag + ">";
		std::string closeTag = "</" + parentTag + ">";

		size_t parentStart = xml.find(openTag);
		if (parentStart == std::string::npos) {
			openTag = "<" + parentTag + ">";
			parentStart = xml.find(openTag);
			if (parentStart == std::string::npos) {
				return "";
			}
		}
		parentStart += openTag.length();

		size_t parentEnd = xml.find(closeTag, parentStart);
		if (parentEnd == std::string::npos) {
			return "";
		}

		std::string parentContent = xml.substr(parentStart, parentEnd - parentStart);

		return ExtractTagValue(parentContent, "str");
	}

	std::string MusicDatabase::ExtractNestedPath(const std::string& xml, const std::string& parentTag) {
		std::string openTag = "<" + parentTag + ">";
		std::string closeTag = "</" + parentTag + ">";

		size_t parentStart = xml.find(openTag);
		if (parentStart == std::string::npos) {
			return "";
		}
		parentStart += openTag.length();

		size_t parentEnd = xml.find(closeTag, parentStart);
		if (parentEnd == std::string::npos) {
			return "";
		}

		std::string parentContent = xml.substr(parentStart, parentEnd - parentStart);

		return ExtractTagValue(parentContent, "path");
	}

}
