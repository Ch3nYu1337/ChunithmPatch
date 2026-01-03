#pragma once
#include <cstdint>
#include <Windows.h>

namespace Chunithm {
namespace Hooks {

bool Initialize(uintptr_t moduleBase);

void Shutdown();

constexpr uintptr_t HOOK_GAME_UPDATE = 0x85C470;

typedef void(__thiscall* GameUpdateFunc)(void* thisPtr, void* param1);
extern GameUpdateFunc OriginalGameUpdate;

typedef char* (__thiscall* ScoreFunc)(void* thisPtr, int param1);
extern ScoreFunc OriginalScore;

typedef int(__thiscall* GetObjectFunc)(void* thisPtr, unsigned int param1);
extern GetObjectFunc OriginalGetObject;

}
}
