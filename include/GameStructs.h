#pragma once
#include <cstdint>
#include <string>

namespace Chunithm {

struct CurrentGameState {
    bool isIniting = true;
    bool isInLobby = false;

    bool isPlaying = false;
    int32_t currentMusicId = 0;
    int32_t currentLevel = 0;

    int32_t currentScore = 0;
    int32_t maxCombo = 0;
    std::string musicName;

    std::string difficulty;

};

extern CurrentGameState g_gameState;

inline const char* GetDifficultyName(int level) {
    switch (level) {
        case 0: return "BASIC: 初级";
        case 1: return "ADVANCED: 高级";
        case 2: return "EXPERT: 专家";
        case 3: return "MASTER: 大师";
        case 4: return "ULTIMA: 究极";
        case 5: return "WORLD'S END";
        default: return "UNKNOWN";
    }
}

inline const char* GetRankName(int score) {
    if (score >= 1009000) return "SSS+";
    if (score >= 1007500) return "SSS";
    if (score >= 1005000) return "SS+";
    if (score >= 1000000) return "SS";
    if (score >= 990000) return "S+";
    if (score >= 975000) return "S";
    if (score >= 950000) return "AAA";
    if (score >= 925000) return "AA";
    if (score >= 900000) return "A";
    if (score >= 800000) return "BBB";
    if (score >= 700000) return "BB";
    if (score >= 600000) return "B";
    if (score >= 500000) return "C";
    return "D";
}

}
