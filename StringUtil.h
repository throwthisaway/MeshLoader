#pragma once
#include <string>
#include <locale>
std::wstring s2ws(const std::string& str);
std::string ws2s(const std::wstring& wstr);
int istrcmp(const std::wstring& str1, size_t offset, const std::wstring& str2);
std::wstring extractFilename(const std::wstring& path);
