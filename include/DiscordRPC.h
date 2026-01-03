#pragma once
#include <cstdint>
#include <string>

namespace Chunithm {

class DiscordRPC {
public:
    static bool Initialize(const char* applicationId);

    static void Shutdown();

    static void UpdatePresence();

    static void ClearPresence();

    static bool IsConnected();

    static void RunCallbacks();

private:
    static bool s_initialized;
    static bool s_connected;
    static int64_t s_startTime;
};

}
