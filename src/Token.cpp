#include "Token.h"

#include <map>

namespace raft {

Token::Token(TokenType type, std::string lexeme, Object literal, std::size_t line)
    : type{type}
    , lexeme{lexeme}
    , literal{literal}
    , line{line}
{
}

std::string Token::toString()
{
    return type2str(type) + " " + lexeme + " " + obj2str(literal);
}

std::string Token::type2str(TokenType tt)
{
    // TODO consider using https://github.com/Neargye/magic_enum
    using Map = std::map<TokenType, std::string>;
    static Map map = {{TokenType::Unrecognized, "Unrecognized"},
                      {TokenType::Space, "Space"},
                      {TokenType::ExclamationMark, "ExclamationMark"},
                      {TokenType::QuotationMark, "QuotationMark"},
                      {TokenType::NumberSign, "NumberSign"},
                      {TokenType::DollarSign, "DollarSign"},
                      {TokenType::PercentSign, "PercentSign"},
                      {TokenType::Ampersand, "Ampersand"},
                      {TokenType::Apostrophe, "Apostrophe"},
                      {TokenType::LeftParenthesis, "LeftParenthesis"},
                      {TokenType::RightParenthesis, "RightParenthesis"},
                      {TokenType::Asterisk, "Asterisk"},
                      {TokenType::PlusSign, "PlusSign"},
                      {TokenType::Comma, "Comma"},
                      {TokenType::HyphenMinus, "HyphenMinus"},
                      {TokenType::FullStop, "FullStop"},
                      {TokenType::Solidus, "Solidus"},
                      {TokenType::Colon, "Colon"},
                      {TokenType::Semicolon, "Semicolon"},
                      {TokenType::LessThanSign, "LessThanSign"},
                      {TokenType::EqualsSign, "EqualsSign"},
                      {TokenType::GreaterThanSign, "GreaterThanSign"},
                      {TokenType::QuestionMark, "QuestionMark"},
                      {TokenType::CommercialAt, "CommercialAt"},
                      {TokenType::LeftSquareBracket, "LeftSquareBracket"},
                      {TokenType::ReverseSolidus, "ReverseSolidus"},
                      {TokenType::RightSquareBracket, "RightSquareBracket"},
                      {TokenType::CircumflexAccent, "CircumflexAccent"},
                      {TokenType::LowLine, "LowLine"},
                      {TokenType::GraveAccent, "GraveAccent"},
                      {TokenType::LeftCurlyBracket, "LeftCurlyBracket"},
                      {TokenType::VerticalLine, "VerticalLine"},
                      {TokenType::RightCurlyBracket, "RightCurlyBracket"},
                      {TokenType::Tilde, "Tilde"},
                      {TokenType::BangEqual, "BangEqual"},
                      {TokenType::EqualEqual, "EqualEqual"},
                      {TokenType::LessEqual, "LessEqual"},
                      {TokenType::GreaterEqual, "GreaterEqual"},
                      {TokenType::Identifier, "Identifier"},
                      {TokenType::StringLiteral, "StringLiteral"},
                      {TokenType::NumberLiteral, "NumberLiteral"},
                      {TokenType::And, "And"},
                      {TokenType::Class, "Class"},
                      {TokenType::Else, "Else"},
                      {TokenType::False, "False"},
                      {TokenType::Fun, "Fun"},
                      {TokenType::For, "For"},
                      {TokenType::If, "If"},
                      {TokenType::Nil, "Nil"},
                      {TokenType::Or, "Or"},
                      {TokenType::Print, "Print"},
                      {TokenType::Return, "Return"},
                      {TokenType::Super, "Super"},
                      {TokenType::This, "This"},
                      {TokenType::True, "True"},
                      {TokenType::Var, "Var"},
                      {TokenType::While, "While"},
                      {TokenType::EndOfFile, "EndOfFile"}};

    return map.at(tt);
}

std::string Token::obj2str(const Object &obj)
{
    auto visitor = [](const auto &arg) -> std::string {
        using T = std::decay_t<decltype(obj)>;
        std::string ret;
        if constexpr (std::is_same_v<T, String>) {
            ret = arg;
        } else if constexpr (std::is_same_v<T, Number>) {
            ret = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, Null>) {
            ret = "nil";
        }
        return ret;
    };
    return std::visit(visitor, obj);
}
}  // namespace raft
