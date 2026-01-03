#include "JacketProvider.h"
#include <fstream>
#include <sstream>
#include <cstdio>

namespace Chunithm {

	std::unordered_map<int32_t, JacketEntry> JacketProvider::s_jacketData;

	static std::string ExtractJsonString(const std::string& json, const std::string& key, size_t startPos = 0) {
		std::string searchKey = "\"" + key + "\"";
		size_t keyPos = json.find(searchKey, startPos);
		if (keyPos == std::string::npos) return "";

		size_t colonPos = json.find(':', keyPos + searchKey.length());
		if (colonPos == std::string::npos) return "";

		size_t quoteStart = json.find('"', colonPos + 1);
		if (quoteStart == std::string::npos) return "";

		size_t quoteEnd = quoteStart + 1;
		while (quoteEnd < json.length()) {
			if (json[quoteEnd] == '"' && json[quoteEnd - 1] != '\\') {
				break;
			}
			quoteEnd++;
		}

		return json.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
	}

	bool JacketProvider::ParseJsonData(const std::string& jsonData) {
		s_jacketData.clear();

		size_t pos = 0;
		while ((pos = jsonData.find('{', pos)) != std::string::npos) {
			size_t endPos = jsonData.find('}', pos);
			if (endPos == std::string::npos) break;

			std::string entry = jsonData.substr(pos, endPos - pos + 1);

			std::string idx = ExtractJsonString(entry, "idx");
			std::string title = ExtractJsonString(entry, "title");
			std::string imageUrl = ExtractJsonString(entry, "imageUrl");

			if (!idx.empty() && !imageUrl.empty()) {
				int32_t musicId = std::stoi(idx);
				s_jacketData[musicId] = { title, imageUrl };
			}

			pos = endPos + 1;
		}

		return !s_jacketData.empty();
	}

	bool JacketProvider::LoadJacketDataFromFile(const std::string& filePath) {
		std::ifstream file(filePath);
		if (!file.is_open()) {
			return false;
		}

		std::stringstream buffer;
		buffer << file.rdbuf();
		file.close();

		return ParseJsonData(buffer.str());
	}

	void JacketProvider::LoadJacketData() {
		if (LoadJacketDataFromFile("jacket-urls.json")) {
			return;
		}
	}

	std::string JacketProvider::GetJacketUrl(int32_t musicId) {
		auto it = s_jacketData.find(musicId);
		if (it != s_jacketData.end()) {
			return it->second.imageUrl;
		}

		return "qr_code_dummy";
	}

	bool JacketProvider::IsConfigured() {
		return !s_jacketData.empty();
	}

	size_t JacketProvider::GetJacketCount() {
		return s_jacketData.size();
	}

}
