#include "source.h"

#include <codecvt>
#include <locale>

namespace draft {

std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;

Source::Source(const std::string &utf8)
    : buf{fromStdString(utf8)}
{
}

std::string Source::toStdString(const std::u32string &utf32)
{
    return conv.to_bytes(utf32);
}

std::u32string Source::fromStdString(const std::string &utf8)
{
    return conv.from_bytes(utf8);
}
}  // namespace draft
