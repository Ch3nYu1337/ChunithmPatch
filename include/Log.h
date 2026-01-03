#pragma once
#include <Windows.h>
#include <cstdio>

#define LOG_ENABLED 1

#if LOG_ENABLED
    #define LOG(fmt, ...) \
        do { \
            char _log_buf[512]; \
            sprintf_s(_log_buf, "[ChunithmPatch] " fmt "\n", ##__VA_ARGS__); \
            OutputDebugStringA(_log_buf); \
        } while(0)

    #define LOG_ERROR(fmt, ...) \
        do { \
            char _log_buf[512]; \
            sprintf_s(_log_buf, "[ChunithmPatch][ERROR] " fmt "\n", ##__VA_ARGS__); \
            OutputDebugStringA(_log_buf); \
        } while(0)

    #define LOG_INFO(fmt, ...) \
        do { \
            char _log_buf[512]; \
            sprintf_s(_log_buf, "[ChunithmPatch][INFO] " fmt "\n", ##__VA_ARGS__); \
            OutputDebugStringA(_log_buf); \
        } while(0)
#else
    #define LOG(fmt, ...) ((void)0)
    #define LOG_ERROR(fmt, ...) ((void)0)
    #define LOG_INFO(fmt, ...) ((void)0)
#endif
