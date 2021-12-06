#include "token.h"

namespace raft {

std::map<Token::Type, std::string> Token::typeNames = {{Token::Type::Unrecognized, "Unrecognized"},
                                                       {Token::Type::And, "And"},
                                                       {Token::Type::Bang, "Bang"},
                                                       {Token::Type::BangEqual, "BangEqual"},
                                                       {Token::Type::Class, "Class"},
                                                       {Token::Type::Comma, "Comma"},
                                                       {Token::Type::Dot, "Dot"},
                                                       {Token::Type::Else, "Else"},
                                                       {Token::Type::Equal, "Equal"},
                                                       {Token::Type::EqualEqual, "EqualEqual"},
                                                       {Token::Type::False, "False"},
                                                       {Token::Type::For, "For"},
                                                       {Token::Type::Fun, "Fun"},
                                                       {Token::Type::GreaterEqual, "GreaterEqual"},
                                                       {Token::Type::GreaterThanSign, "GreaterThanSign"},
                                                       {Token::Type::Identifier, "Identifier"},
                                                       {Token::Type::If, "If"},
                                                       {Token::Type::LeftCurlyBracket, "LeftCurlyBracket"},
                                                       {Token::Type::LeftParenthesis, "LeftParenthesis"},
                                                       {Token::Type::LessEqual, "LessEqual"},
                                                       {Token::Type::LessThanSign, "LessThanSign"},
                                                       {Token::Type::Minus, "Minus"},
                                                       {Token::Type::Nil, "Nil"},
                                                       {Token::Type::NumberLiteral, "NumberLiteral"},
                                                       {Token::Type::Or, "Or"},
                                                       {Token::Type::Plus, "Plus"},
                                                       {Token::Type::Print, "Print"},
                                                       {Token::Type::Return, "Return"},
                                                       {Token::Type::RightCurlyBracket, "RightCurlyBracket"},
                                                       {Token::Type::RightParenthesis, "RightParenthesis"},
                                                       {Token::Type::Semicolon, "Semicolon"},
                                                       {Token::Type::Slash, "Slash"},
                                                       {Token::Type::Star, "Star"},
                                                       {Token::Type::StringLiteral, "StringLiteral"},
                                                       {Token::Type::Super, "Super"},
                                                       {Token::Type::This, "This"},
                                                       {Token::Type::True, "True"},
                                                       {Token::Type::Var, "Var"},
                                                       {Token::Type::While, "While"},
                                                       {Token::Type::EndOfFile, "EndOfFile"}};

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
    return typeNames.at(type);
}

}  // namespace raft
