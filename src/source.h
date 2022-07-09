#pragma once

#include <string>
#include <vector>

namespace draft {

class Source {
public:
    using Id = std::uint64_t;
    using Buffer = std::u32string;
    using Char = char32_t;

    using Offset = std::size_t;
    struct Position {
        std::uint32_t line = 0;
        std::uint32_t column = 0;
    };
    struct Location {
        Id id = 0;
        std::string path;
        Position position;
    };

    Source() = default;
    explicit Source(const std::string &utf8);

    Position positionAt(Offset offset) const;
    std::string lineAt(std::uint32_t line) const;
    Char charAt(Offset offset) const;

    static std::string toStdString(const std::u32string &utf32);
    static std::u32string fromStdString(const std::string &utf8);

private:
    void calculateLineOffsets();

    Buffer buf;
    std::vector<Offset> lineOffsets;
};

}  // namespace draft
