#include "ogm/sys/util_sys.hpp"
#include "fs_share.hpp"
#include <iostream>

#if defined(WIN32) || defined(__WIN32__) || defined(_WIN32)
#include <windows.h>
#include <shlwapi.h>

namespace ogm
{
    // these are required to ensure double-null terminated paths.
    #ifndef BUFFLEN
    #define BUFFLEN 1024
    #endif

    bool create_directory(const std::string& path)
    {
        char buff[BUFFLEN];
        if (path.length() >= BUFFLEN)
        {
            return false;
        }
        for (size_t i = 0; i < BUFFLEN; ++i)
        {
            buff[i] = 0;
        }
        strcpy(buff, path.c_str());
        return CreateDirectoryA(buff, nullptr);
    }

    bool directory_exists(const std::string& path)
    {
        char buff[BUFFLEN];
        if (path.length() >= BUFFLEN)
        {
            return false;
        }
        for (size_t i = 0; i < BUFFLEN; ++i)
        {
            buff[i] = 0;
        }
        strcpy(buff, path.c_str());
        DWORD dwAttrib = GetFileAttributesA(buff);

        return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
               (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    bool file_exists(const std::string& path)
    {
        char buff[BUFFLEN];
        if (path.length() >= BUFFLEN)
        {
            return false;
        }
        for (size_t i = 0; i < BUFFLEN; ++i)
        {
            buff[i] = 0;
        }
        strcpy(buff, path.c_str());
        DWORD dwAttrib = GetFileAttributesA(buff);

        return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
               !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    std::vector<std::string> list_directory(const std::string& path)
    {
        std::vector<std::string> result;
        char buff[BUFFLEN];
        if (path.length() + 2 >= BUFFLEN)
        {
            return result;
        }
        for (size_t i = 0; i < BUFFLEN; ++i)
        {
            buff[i] = 0;
        }
        strcpy(buff, path.c_str());
        strcat(buff, "\\*");

        WIN32_FIND_DATAA findData;
        HANDLE hFind = FindFirstFileA(buff, &findData);

        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                result.push_back(findData.cFileName);
            }
            while (FindNextFileA(hFind, &findData));
            FindClose(hFind);
        }

        return result;
    }

    // returns path to the directory containing the executable.
    std::string get_binary_directory()
    {
        const size_t bufsize = 1024;
        char buf[bufsize];
        int32_t len = static_cast<int32_t>(GetModuleFileName(
            nullptr,
            buf,
            bufsize
        ));

        if (len <= 0)
        {
            throw MiscError("Failed to determine binary directory name.");
        }
        else if (len == bufsize)
        {
            buf[bufsize - 1] = 0;
            return path_directory(buf);
        }
        else
        {
            buf[len] = 0;
            return path_directory(buf);
        }
    }
}

#endif
