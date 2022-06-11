#include "raylib.h"
#include "polyfill42.h"
#include <string.h>

// Platform specific defines to handle GetApplicationDirectory()
#if defined (PLATFORM_DESKTOP)
#if defined(_WIN32)
#ifndef MAX_PATH
#define MAX_PATH 1025
#endif
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameA(void* hModule, void* lpFilename, unsigned long nSize);
__declspec(dllimport) unsigned long __stdcall GetModuleFileNameW(void* hModule, void* lpFilename, unsigned long nSize);
__declspec(dllimport) int __stdcall WideCharToMultiByte(unsigned int cp, unsigned long flags, void* widestr, int cchwide, void* str, int cbmb, void* defchar, int* used_default);
#elif defined(__linux__)
#include <unistd.h>
#elif defined(__APPLE__)
#include <sys/syslimits.h>
#include <mach-o/dyld.h>
#endif // OSs

const char* GetApplicationDirectory(void)
{
    static char appDir[MAX_FILEPATH_LENGTH] = { 0 };
    memset(appDir, 0, MAX_FILEPATH_LENGTH);

#if defined(_WIN32)
    int len = 0;
#if defined (UNICODE)
    unsigned short widePath[MAX_PATH];
    len = GetModuleFileNameW(NULL, widePath, MAX_PATH);
    len = WideCharToMultiByte(0, 0, widePath, len, appDir, MAX_PATH, NULL, NULL);
#else
    len = GetModuleFileNameA(0, appDir, MAX_PATH);
#endif
    if (len > 0)
    {
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '\\')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '\\';
    }

#elif defined(__linux__)
    unsigned int size = sizeof(appDir);
    ssize_t len = readlink("/proc/self/exe", appDir, size);

    if (len > 0)
    {
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '/')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '/';
    }
#elif defined(__APPLE__)
    uint32_t size = sizeof(appDir);

    if (_NSGetExecutablePath(appDir, &size) == 0)
    {
        int len = strlen(appDir);
        for (int i = len; i >= 0; --i)
        {
            if (appDir[i] == '/')
            {
                appDir[i + 1] = '\0';
                break;
            }
        }
    }
    else
    {
        appDir[0] = '.';
        appDir[1] = '/';
    }
#endif

    return appDir;
}
#endif // PLATFORM_DESKTOP
