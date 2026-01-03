#include <Windows.h>
#include <MinHook.h>
#include "Hooks.h"
#include "GameStructs.h"
#include "MusicDatabase.h"
#include "DiscordRPC.h"
#include "JacketProvider.h"

#define DISCORD_APP_ID "1456600317230846090"

namespace {

	bool g_running = true;

	bool Initialize() {
		uintptr_t moduleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(nullptr));

		if (MH_Initialize() != MH_OK) {
			return false;
		}

		Chunithm::Hooks::Initialize(moduleBase);

		Chunithm::MusicDatabase::Initialize();

		Chunithm::JacketProvider::LoadJacketData();

		Chunithm::DiscordRPC::Initialize(DISCORD_APP_ID);

		return true;
	}

	void Cleanup() {
		Chunithm::DiscordRPC::Shutdown();
		Chunithm::Hooks::Shutdown();
		MH_Uninitialize();
	}

	DWORD WINAPI MainThread(LPVOID lpParam) {
		if (!Initialize()) {
			FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 1);
			return 1;
		}

		int discordUpdateCounter = 0;

		while (g_running) {
			discordUpdateCounter++;
			if (discordUpdateCounter >= 10) {
				Chunithm::DiscordRPC::RunCallbacks();
				Chunithm::DiscordRPC::UpdatePresence();
				discordUpdateCounter = 0;
			}

			Sleep(100);
		}

		Cleanup();
		FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
		return 0;
	}

}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
		break;

	case DLL_PROCESS_DETACH:
		g_running = false;
		break;
	}
	return TRUE;
}
