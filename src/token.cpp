#include "token.h"

namespace raft {

Token::Token(Type type, std::string lexeme, object::Object literal, std::size_t line)
    : type{type}
    , lexeme{std::move(lexeme)}
    , literal{std::move(literal)}
    , line{line}
{
}

std::string Token::toString()
{
    return type2str(type) + " " + lexeme + " " + object::obj2str(literal);
}

std::string Token::type2str(Token::Type type)
{
    switch (type) {
    case Token::Type::Unrecognized:
        break;
#define TOKEN(name)         \
    case Token::Type::name: \
        return #name;
#include "token.def"
    case Token::Type::EndOfFile:
        return "EndOfFile";
    }
    return "Unrecognized";
}

}  // namespace raft
