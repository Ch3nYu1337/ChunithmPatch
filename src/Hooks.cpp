#include "Hooks.h"
#include "GameStructs.h"
#include "MusicDatabase.h"
#include "Log.h"
#include <MinHook.h>
#include <intrin.h>

namespace Chunithm {

	CurrentGameState g_gameState;

	namespace Hooks {

		constexpr uintptr_t HOOK_SCORE = 0x25E2D;
		constexpr uintptr_t HOOK_GET_OBJECT = 0x68A16;
		constexpr uintptr_t RET_ADDR_LOBBY = 0xD96F12;
		constexpr uintptr_t RET_ADDR_INIT = 0xD8CF08;

		GameUpdateFunc OriginalGameUpdate = nullptr;
		ScoreFunc OriginalScore = nullptr;
		GetObjectFunc OriginalGetObject = nullptr;

		static uintptr_t s_moduleBase = 0;

		static bool CreateAndEnableHook(void* target, void* detour, void** original, const char* name) {
			MH_STATUS status = MH_CreateHook(target, detour, original);
			if (status != MH_OK) {
				LOG_ERROR("Failed to create %s hook at 0x%p, status: %d", name, target, status);
				return false;
			}

			status = MH_EnableHook(target);
			if (status != MH_OK) {
				LOG_ERROR("Failed to enable %s hook, status: %d", name, status);
				return false;
			}

			LOG_INFO("Successfully hooked %s at 0x%p", name, target);
			return true;
		}

		void __fastcall HookedGameUpdate(void* thisPtr, void* edx, void* param1) {
			if (OriginalGameUpdate) {
				OriginalGameUpdate(thisPtr, param1);
			}

			if (param1) {
				uint8_t* basePtr = reinterpret_cast<uint8_t*>(param1);
				int32_t musicId = *reinterpret_cast<int32_t*>(basePtr + 0x10);

				if (musicId > 0 && musicId < 10000) {
					g_gameState.isPlaying = false;
					g_gameState.currentMusicId = musicId;
					g_gameState.currentLevel = *reinterpret_cast<int8_t*>(basePtr + 0x14);
					g_gameState.difficulty = GetDifficultyName(g_gameState.currentLevel);
					g_gameState.musicName = MusicDatabase::GetMusicName(musicId);
				}
			}
		}

		char* __fastcall HookedScore(void* thisPtr, void* edx, int param1) {
			char* ret = nullptr;
			if (OriginalScore) {
				ret = OriginalScore(thisPtr, param1);
			}

			g_gameState.isPlaying = true;
			g_gameState.currentScore = *reinterpret_cast<int32_t*>(param1 + 0xf4);
			g_gameState.maxCombo = *reinterpret_cast<int32_t*>(param1 + 0xe8);

			return ret;
		}

		int __fastcall HookedGetObject(void* thisPtr, void* edx, unsigned int param1) {
			int ret = 0;
			if (OriginalGetObject) {
				ret = OriginalGetObject(thisPtr, param1);
			}

			uintptr_t retAddr = reinterpret_cast<uintptr_t>(_ReturnAddress());

			if (retAddr == RET_ADDR_LOBBY) {
				g_gameState.isIniting = false;
				g_gameState.isInLobby = true;
			}
			else if (retAddr == RET_ADDR_INIT) {
				g_gameState.isIniting = true;
				g_gameState.isInLobby = false;
				g_gameState.currentMusicId = 0;
			}

			return ret;
		}

		bool Initialize(uintptr_t moduleBase) {
			s_moduleBase = moduleBase;

			void* gameUpdateAddr = reinterpret_cast<void*>(moduleBase + HOOK_GAME_UPDATE);
			if (!CreateAndEnableHook(gameUpdateAddr, &HookedGameUpdate,
				reinterpret_cast<void**>(&OriginalGameUpdate), "GameUpdate")) {
				return false;
			}

			void* scoreAddr = reinterpret_cast<void*>(moduleBase + HOOK_SCORE);
			if (!CreateAndEnableHook(scoreAddr, &HookedScore,
				reinterpret_cast<void**>(&OriginalScore), "Score")) {
				return false;
			}

			void* getObjectAddr = reinterpret_cast<void*>(moduleBase + HOOK_GET_OBJECT);
			if (!CreateAndEnableHook(getObjectAddr, &HookedGetObject,
				reinterpret_cast<void**>(&OriginalGetObject), "GetObject")) {
				return false;
			}

			return true;
		}

		void Shutdown() {
			void* gameUpdateAddr = reinterpret_cast<void*>(s_moduleBase + HOOK_GAME_UPDATE);
			MH_DisableHook(gameUpdateAddr);
			MH_RemoveHook(gameUpdateAddr);

			void* scoreAddr = reinterpret_cast<void*>(s_moduleBase + HOOK_SCORE);
			MH_DisableHook(scoreAddr);
			MH_RemoveHook(scoreAddr);

			void* getObjectAddr = reinterpret_cast<void*>(s_moduleBase + HOOK_GET_OBJECT);
			MH_DisableHook(getObjectAddr);
			MH_RemoveHook(getObjectAddr);

			LOG_INFO("Hooks disabled and cleaned up");
		}

	}
}
