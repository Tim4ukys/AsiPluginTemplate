/*
    AsiPluginTemplate source file
    Authors: tim4ukys
    url: https://github.com/Tim4ukys/AsiPluginTemplate
    tg: https://t.me/tim4ukys
    Do not delete this comment block. Respect others' work!
*/
#include "snippets.hpp"
#include <sstream>

std::string base::snippets::CP1251_to_UTF8(std::string_view str) {
    if (!str.empty()) {
        int wchlen = MultiByteToWideChar(1251, 0, str.data(), str.size(), NULL, 0);
        if (wchlen > 0 && wchlen != 0xFFFD) {
            std::wstring wbuff;
            wbuff.resize(wchlen);
            MultiByteToWideChar(1251, 0, str.data(), str.size(), wbuff.data(), wbuff.size());
            std::string buff;
            buff.resize(wchlen);
            WideCharToMultiByte(CP_UTF8, 0, wbuff.data(), wbuff.size(), buff.data(), buff.size(), 0, 0);
            return buff;
        }
    }
    return std::string();
}

std::string base::snippets::UTF8_to_CP1251(std::string_view utf8) {
    if (!utf8.empty()) {
        int wchlen = MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size(), NULL, 0);
        if (wchlen > 0 && wchlen != 0xFFFD) {
            std::wstring wbuff;
            wbuff.resize(wchlen);
            MultiByteToWideChar(CP_UTF8, 0, utf8.data(), utf8.size(), wbuff.data(), wchlen);
            std::string buff;
            buff.resize(wchlen);
            WideCharToMultiByte(1251, 0, wbuff.data(), wchlen, buff.data(), wchlen, 0, 0);
            return buff;
        }
    }
    return std::string();
}

std::string base::snippets::ConvertWideToANSI(std::wstring_view wstr) {
    if (!wstr.empty()) {
        int         len = WideCharToMultiByte(CP_ACP, 0, wstr.data(), wstr.size(), NULL, 0, NULL, NULL);
        std::string str;
        str.resize(len);
        WideCharToMultiByte(CP_ACP, 0, wstr.data(), wstr.size(), str.data(), len, NULL, NULL);
        return str;
    }
    return std::string();
}

std::array<int, 3> base::snippets::versionParse(const std::string& vers) {
    const std::regex r{R"(([\w]+)\.([\w]+)\.([\w]+))"};
    std::smatch      m;

    std::array<int, 3> resolve;
    if (std::regex_search(vers, m, r)) {
        for (size_t i{0}; i < 3;) {
            resolve[i++] = std::atoi(m[i + 1].str().c_str());
        }
    }
    return resolve;
}

#pragma warning(push)
#pragma warning(disable : 4244)
std::string base::snippets::GetSystemFontPath(const std::string& faceName) {
    static const wchar_t* fontRegistryPath = L"Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts";
    HKEY                hKey;
    LONG                result;
    std::wstring        wsFaceName{faceName.begin(), faceName.end()};
    std::wstring        wsFontFile;

    // Open Windows font registry key
    result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, fontRegistryPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        goto chk_local;
    }

    DWORD maxValueNameSize, maxValueDataSize;
    result = RegQueryInfoKeyW(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
    if (result != ERROR_SUCCESS) {
        goto chk_local;
    }

{
        DWORD  valueIndex = 0;
        LPWSTR valueName = new WCHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD  valueNameSize, valueDataSize, valueType;

        // Look for a matching font name
        do {

            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValueW(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::wstring wsValueName(valueName, valueNameSize);

            // Found a match
            if (_wcsnicmp(wsFaceName.c_str(), wsValueName.c_str(), wsFaceName.length()) == 0 &&
                wsFaceName.length() + ARRAYSIZE(" (TrueType)") - 1 == wsValueName.length()) {

                wsFontFile.assign((LPWSTR)valueData, valueDataSize);
                break;
            }
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;
    }
    RegCloseKey(hKey);

    if (wsFontFile.empty()) {
        goto chk_local;
    }

    // Build full font file path
    wchar_t winDir[MAX_PATH];
    GetWindowsDirectoryW(winDir, MAX_PATH);

    {
        std::wstringstream ss{};
        ss << winDir << "\\Fonts\\" << wsFontFile;
        wsFontFile = ss.str();
    }

    return std::string(wsFontFile.begin(), wsFontFile.end());

 chk_local:

    result = RegOpenKeyExW(HKEY_CURRENT_USER, fontRegistryPath, 0, KEY_READ, &hKey);
    if (result != ERROR_SUCCESS) {
        return "";
    }

    result = RegQueryInfoKeyW(hKey, 0, 0, 0, 0, 0, 0, 0, &maxValueNameSize, &maxValueDataSize, 0, 0);
    if (result != ERROR_SUCCESS) {
        return "";
    }

{
        DWORD  valueIndex = 0;
        LPWSTR valueName = new WCHAR[maxValueNameSize];
        LPBYTE valueData = new BYTE[maxValueDataSize];
        DWORD  valueNameSize, valueDataSize, valueType;

        // Look for a matching font name
        do {

            wsFontFile.clear();
            valueDataSize = maxValueDataSize;
            valueNameSize = maxValueNameSize;

            result = RegEnumValueW(hKey, valueIndex, valueName, &valueNameSize, 0, &valueType, valueData, &valueDataSize);

            valueIndex++;

            if (result != ERROR_SUCCESS || valueType != REG_SZ) {
                continue;
            }

            std::wstring wsValueName(valueName, valueNameSize);

            // Found a match
            if (_wcsnicmp(wsFaceName.c_str(), wsValueName.c_str(), wsFaceName.length()) == 0 &&
                wsFaceName.length() + ARRAYSIZE(" (TrueType)") - 1 == wsValueName.length()) {

                wsFontFile.assign((LPWSTR)valueData, valueDataSize);
                break;
            }
        } while (result != ERROR_NO_MORE_ITEMS);

        delete[] valueName;
        delete[] valueData;
    }

    RegCloseKey(hKey);

    if (wsFontFile.empty()) {
        return "";
    }

    return std::string(wsFontFile.begin(), wsFontFile.end());
}
#pragma warning(pop)