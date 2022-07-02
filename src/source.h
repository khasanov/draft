#pragma once

#include <string>

namespace draft {

class Source {
public:
    static std::string toStdString(const std::u32string &utf32);
    static std::u32string fromStdString(const std::string &utf8);
};

}  // namespace draft
