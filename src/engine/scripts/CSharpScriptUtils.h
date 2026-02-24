#pragma once

static auto wstringToString(const std::wstring &wstr) -> std::string
{
    std::string str(wstr.begin(), wstr.end());
    return str;
}

#if defined(WINDOWS)
#define CHAR_PTR__TO_STRING(S) wstringToString(std::wstring(S))
#else
#define CHAR_PTR__TO_STRING(S) std::string(S)
#endif