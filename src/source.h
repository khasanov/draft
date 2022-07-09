#pragma once

#include <string>

namespace draft {

class Source {
public:
    using Id = std::uint64_t;
    using Buffer = std::u32string;

    Source() = default;
    explicit Source(const std::string &utf8);

    static std::string toStdString(const std::u32string &utf32);
    static std::u32string fromStdString(const std::string &utf8);

private:
    Buffer buf;
};

}  // namespace draft
