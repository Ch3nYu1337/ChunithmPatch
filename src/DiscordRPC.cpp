#include "DiscordRPC.h"
#include "GameStructs.h"
#include "MusicDatabase.h"
#include "JacketProvider.h"
#include <discord_rpc.h>
#include <cstdio>
#include <ctime>

namespace Chunithm {

	bool DiscordRPC::s_initialized = false;
	bool DiscordRPC::s_connected = false;
	int64_t DiscordRPC::s_startTime = 0;

	static bool g_discordConnected = false;

	static void HandleDiscordReady(const DiscordUser* user) {
		g_discordConnected = true;
	}

	static void HandleDiscordDisconnected(int errorCode, const char* message) {
		g_discordConnected = false;
	}

	static void HandleDiscordError(int errorCode, const char* message) {
	}

	bool DiscordRPC::Initialize(const char* applicationId) {
		if (s_initialized) {
			return true;
		}

		DiscordEventHandlers handlers;
		memset(&handlers, 0, sizeof(handlers));
		handlers.ready = HandleDiscordReady;
		handlers.disconnected = HandleDiscordDisconnected;
		handlers.errored = HandleDiscordError;

		Discord_Initialize(applicationId, &handlers, 1, nullptr);

		s_startTime = time(nullptr);
		s_initialized = true;

		return true;
	}

	void DiscordRPC::Shutdown() {
		if (s_initialized) {
			ClearPresence();
			Discord_Shutdown();
			s_initialized = false;
			s_connected = false;
		}
	}

	void DiscordRPC::UpdatePresence() {
		if (!s_initialized) {
			return;
		}

		DiscordRichPresence presence;
		memset(&presence, 0, sizeof(presence));

		static char stateBuffer[128];
		static char detailsBuffer[128];
		static char largeImageKey[512];
		static char largeImageText[128];
		static char smallImageText[64];

		if (g_gameState.currentMusicId > 0 && g_gameState.isInLobby && !g_gameState.isIniting) {
			std::string musicName = g_gameState.musicName;
			if (musicName.empty()) {
				musicName = "Music ID: " + std::to_string(g_gameState.currentMusicId);
			}

			snprintf(detailsBuffer, sizeof(detailsBuffer), "%s", musicName.c_str());
			presence.details = detailsBuffer;

			if (g_gameState.isPlaying) {
				snprintf(stateBuffer, sizeof(stateBuffer), "%s | Score: %d | %s",
					g_gameState.difficulty.c_str(),
					g_gameState.currentScore,
					GetRankName(g_gameState.currentScore));
			}
			else {
				snprintf(stateBuffer, sizeof(stateBuffer), "%s", g_gameState.difficulty.c_str());
			}
			presence.state = stateBuffer;

			std::string jacketUrl = JacketProvider::GetJacketUrl(g_gameState.currentMusicId);
			if (!jacketUrl.empty()) {
				snprintf(largeImageKey, sizeof(largeImageKey), "%s", jacketUrl.c_str());
				presence.largeImageKey = largeImageKey;
			}
			else {
				presence.largeImageKey = "qr_code_dummy";
			}
			snprintf(largeImageText, sizeof(largeImageText), "%s", musicName.c_str());
			presence.largeImageText = largeImageText;

			snprintf(smallImageText, sizeof(smallImageText), "%s", g_gameState.difficulty.c_str());
			presence.smallImageText = smallImageText;
		}
		else if (g_gameState.isIniting) {
			presence.details = "Welcome to Chunithm!";
			presence.state = "Initializing...";
			presence.largeImageKey = "qr_code_dummy";
			presence.largeImageText = "CHUNITHM";
		}
		else if (g_gameState.isInLobby) {
			presence.details = "Welcome to Chunithm!";
			presence.state = "In Lobby";
			presence.largeImageKey = "qr_code_dummy";
			presence.largeImageText = "CHUNITHM";
		}
		else {
			presence.details = "CHUNITHM";
			presence.state = "In Idle";
			presence.largeImageKey = "qr_code_dummy";
			presence.largeImageText = "CHUNITHM";
		}


		presence.startTimestamp = s_startTime;

		Discord_UpdatePresence(&presence);
	}

	void DiscordRPC::ClearPresence() {
		if (s_initialized) {
			Discord_ClearPresence();
		}
	}

	bool DiscordRPC::IsConnected() {
		return g_discordConnected;
	}

	void DiscordRPC::RunCallbacks() {
		if (s_initialized) {
			Discord_RunCallbacks();
		}
	}

}
