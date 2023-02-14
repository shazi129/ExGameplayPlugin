#pragma once

#include <string>

class FMiniCodeConvert {
public:
    static std::string WideCharToMultiByte(const TCHAR* WChar);
    static FString MultiByteToWideChar(const char* MultiByteChar);
};