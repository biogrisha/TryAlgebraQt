#pragma once
#include "pathsConfig.h"
#include <windows.h>
#include <string>
#include <filesystem>

namespace FSUtils
{
    inline std::string getExecutableDir() {
        wchar_t buffer[MAX_PATH];
        DWORD size = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
        std::filesystem::path exePath(buffer, buffer + size);
        auto result = exePath.parent_path().string();
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    inline std::string getSourceDir()
    {
        return LIB_SOURCE_ROOT;
    }

    inline std::string getAssetsPath()
    {

        return getExecutableDir();
    }
}