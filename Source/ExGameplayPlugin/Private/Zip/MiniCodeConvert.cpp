
#include "MiniCodeConvert.h"
#include "Internationalization/Culture.h"

#include <codecvt>
#include <locale>

template <class Elem, class Byte = char, class Statype = std::mbstate_t>
class TCodeConvertByName : public std::codecvt<Elem, Byte, Statype> {
public:
    explicit TCodeConvertByName(const char* Locname, size_t Refs = 0)
        : std::codecvt<Elem, Byte, Statype>(Locname, Refs) {} // 根据语言名称构造编码转换结构

    explicit TCodeConvertByName(const std::string& Str, size_t Refs = 0)
        : std::codecvt<Elem, Byte, Statype>(Str.c_str(), Refs) {} // 根据语言名称构造编码转换结构

    virtual ~TCodeConvertByName() {}
};

typedef TCodeConvertByName<TCHAR> FCodeConvertByName;

std::string FMiniCodeConvert::WideCharToMultiByte(const TCHAR* WChar)
{
#if PLATFORM_WINDOWS // Windows 默认使用的是ANSI编码
    const std::string& OSLang = TCHAR_TO_UTF8(*FInternationalization::Get().GetDefaultLanguage()->GetName());
    static std::wstring_convert<FCodeConvertByName, TCHAR> StrCnv(new FCodeConvertByName(OSLang));

    try
    {
        return std::string(StrCnv.to_bytes(WChar));
    }
    catch (std::exception)
    {
        return TCHAR_TO_UTF8(WChar);
    }
#endif

    return TCHAR_TO_UTF8(WChar);
}

FString FMiniCodeConvert::MultiByteToWideChar(const char* MultiByteChar)
{
#if PLATFORM_WINDOWS // Windows 默认使用的是ANSI编码
    const std::string& OSLang = TCHAR_TO_UTF8(*FInternationalization::Get().GetDefaultLanguage()->GetName());
    static std::wstring_convert<FCodeConvertByName, TCHAR> StrCnv(new FCodeConvertByName(OSLang));

    try
    {
        return StrCnv.from_bytes(MultiByteChar).c_str();
    }
    catch (std::exception)
    {
        return UTF8_TO_TCHAR(MultiByteChar);
    }
#endif

    return UTF8_TO_TCHAR(MultiByteChar);
}