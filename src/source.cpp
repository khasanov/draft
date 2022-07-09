#include "source.h"

#include <codecvt>
#include <locale>

namespace draft {

std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;

Source::Source(const std::string &utf8)
    : buf{fromStdString(utf8)}
{
    calculateLineOffsets();
}

Source::Position Source::positionAt(Offset offset) const
{
    Position pos{};

    if (buf.empty()) {
        return pos;
    }

    offset = std::min(offset, buf.size() - 1);

    auto it = std::upper_bound(lineOffsets.begin(), lineOffsets.end(), offset);
    auto index = std::distance(lineOffsets.begin(), std::prev(it));

    pos.line = index + 1;
    pos.column = offset - lineOffsets.at(index) + 1;
    return pos;
}

std::string Source::lineAt(std::uint32_t line) const
{
    if (buf.empty()) {
        return "";
    }

    line -= 1;  // expect lines numbering from one
    if (line >= lineOffsets.size()) {
        return "";
    }

    auto begin = lineOffsets.at(line);
    auto end = line == lineOffsets.size() - 1 ? buf.size() : lineOffsets.at(line + 1);
    Buffer utf32 = buf.substr(begin, end - begin);
    return toStdString(utf32);
}

Source::Char Source::charAt(Offset offset) const
{
    if (offset < buf.size()) {
        return buf.at(offset);
    }
    return '\0';
}

std::string Source::toStdString(const std::u32string &utf32)
{
    return conv.to_bytes(utf32);
}

std::u32string Source::fromStdString(const std::string &utf8)
{
    return conv.from_bytes(utf8);
}

void Source::calculateLineOffsets()
{
    lineOffsets.clear();
    bool isLineStart = true;
    for (Offset i = 0; i < buf.size(); ++i) {
        if (isLineStart) {
            lineOffsets.push_back(i);
        }
        isLineStart = buf.at(i) == '\n';
    }
    if (isLineStart and buf.size() != 0) {
        lineOffsets.push_back(buf.size());
    }
}
}  // namespace draft
