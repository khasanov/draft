#include <iomanip>
#include <iostream>
#include <map>

#include "scanner.h"
#include "raft.h"

namespace raft {

Scanner::Scanner(std::string_view source)
    : source{source}
{
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme
        start = current;
        scanToken();
    }

    Token eof{Token::Type::EndOfFile, "", object::Null{}, line};
    tokens.emplace_back(std::move(eof));
    return tokens;
}

bool Scanner::isAtEnd()
{
    return current >= source.length();
}

void Scanner::scanToken()
{
    char c = advance();
    switch (c) {
    case '(':
        addToken(Token::Type::LeftParenthesis);
        break;
    case ')':
        addToken(Token::Type::RightParenthesis);
        break;
    case '{':
        addToken(Token::Type::LeftCurlyBracket);
        break;
    case '}':
        addToken(Token::Type::RightCurlyBracket);
        break;
    case ',':
        addToken(Token::Type::Comma);
        break;
    case '.':
        addToken(Token::Type::Dot);
        break;
    case '-':
        addToken(Token::Type::Minus);
        break;
    case '+':
        addToken(Token::Type::Plus);
        break;
    case ';':
        addToken(Token::Type::Semicolon);
        break;
    case '*':
        addToken(Token::Type::Star);
        break;
    case '!':
        addToken(match('=') ? Token::Type::BangEqual : Token::Type::Bang);
        break;
    case '=':
        addToken(match('=') ? Token::Type::EqualEqual : Token::Type::Equal);
        break;
    case '<':
        addToken(match('=') ? Token::Type::LessEqual : Token::Type::LessThanSign);
        break;
    case '>':
        addToken(match('=') ? Token::Type::GreaterEqual : Token::Type::GreaterThanSign);
        break;
    case '/':
        if (match('/')) {
            // A comment goes until the end of the line
            while (peek() != '\n' and !isAtEnd()) {
                advance();
            }
        } else {
            addToken(Token::Type::Slash);
        }
        break;
    case ' ':
        [[fallthrough]];
    case '\r':
        [[fallthrough]];
    case '\t':
        // Ignore whitespace
        break;
    case '\n':
        line++;
        break;

    case '"':
        string();
        break;
    default:
        if (isDigit(c)) {
            number();
        } else if (isAlpha(c)) {
            identifier();
        } else {
            std::string symbol = "'" + std::string{1, c} + "' (" + std::to_string(c) + ")";
            std::string msg = "Unexpected character " + symbol + " at column " + std::to_string(current);
            Raft::error(line, msg);
        }
        break;
    }
}

char Scanner::advance()
{
    return source.at(current++);
}

bool Scanner::match(char expected)
{
    if (isAtEnd()) {
        return false;
    }
    if (source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
    ;
}

char Scanner::peek()
{
    if (isAtEnd()) {
        return '\0';
    }
    return source.at(current);
}

char Scanner::peekNext()
{
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source.at(current + 1);
}

void Scanner::string()
{
    while (peek() != '"' and not isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }

    if (isAtEnd()) {
        Raft::error(line, "Unterminated string");
        return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    auto value = source.substr(start + 1, current - 1);
    addToken(Token::Type::StringLiteral, std::string{value});
}

void Scanner::number()
{
    while (isDigit(peek())) {
        advance();
    }

    // Look for a fractional part
    if (peek() == '.' and isDigit(peekNext())) {
        // Consume the "."
        advance();

        while (isDigit(peek())) {
            advance();
        }
    }

    double value = std::stod(std::string{source.substr(start, current)});
    addToken(Token::Type::NumberLiteral, value);
}

void Scanner::identifier()
{
    static std::map<std::string, Token::Type> keywords = {
        {"and", Token::Type::And},     {"class", Token::Type::Class},   {"else", Token::Type::Else},
        {"false", Token::Type::False}, {"fun", Token::Type::Fun},       {"for", Token::Type::For},
        {"if", Token::Type::If},       {"nil", Token::Type::Nil},       {"or", Token::Type::Or},
        {"print", Token::Type::Print}, {"return", Token::Type::Return}, {"super", Token::Type::Super},
        {"this", Token::Type::This},   {"true", Token::Type::True},     {"var", Token::Type::Var},
        {"while", Token::Type::While}};

    while (isAlphaNumeric(peek())) {
        advance();
    }
    auto text = source.substr(start, current);
    if (auto it = keywords.find(std::string{text}); it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(Token::Type::Identifier);
    }
}

void Scanner::addToken(Token::Type type)
{
    addToken(type, object::Null{});
}

void Scanner::addToken(Token::Type type, object::Object literal)
{
    auto text = source.substr(start, current - start);
    Token t{type, std::string{text}, literal, line};
    tokens.emplace_back(std::move(t));
}

bool Scanner::isDigit(char c)
{
    return c >= '0' and c <= '9';
}

bool Scanner::isAlpha(char c)
{
    return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_';
}

bool Scanner::isAlphaNumeric(char c)
{
    return isAlpha(c) or isDigit(c);
}

}  // namespace raft
