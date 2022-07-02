#include "token.h"

namespace draft {

static std::string kind2str(Token::Kind kind)
{
    switch (kind) {
    case Token::Kind::Unrecognized:
        break;
#define TOKEN(name)         \
    case Token::Kind::name: \
        return #name;
#include "token.def"
    case Token::Kind::EndOfFile:
        return "EndOfFile";
    }
    return "Unrecognized";
}

Token::Token(Kind kind, Lexeme lexeme, object::Object literal, std::size_t line)
    : kind{kind}
    , lexeme{std::move(lexeme)}
    , literal{std::move(literal)}
    , line{line}
{
}

std::string Token::toString()
{
    return kind2str(kind) + " " + lexeme + " " + object::obj2str(literal);
}

}  // namespace draft
