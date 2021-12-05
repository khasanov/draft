#include <iomanip>
#include <iostream>
#include <map>

#include "scanner.h"

namespace raft {

Scanner::Scanner(std::string_view source)
    : source{source}
{
}

std::vector<Token> Scanner::scanTokens()
{
    while (!isAtEnd()) {
        start = current;
        scanToken();
    }

    Token eof{TokenType::EndOfFile, "", Null{}, line};
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
        addToken(TokenType::LeftParenthesis);
        break;
    case ')':
        addToken(TokenType::RightParenthesis);
        break;
    case '{':
        addToken(TokenType::LeftCurlyBracket);
        break;
    case '}':
        addToken(TokenType::RightCurlyBracket);
        break;
    case ',':
        addToken(TokenType::Comma);
        break;
    case '.':
        addToken(TokenType::FullStop);
        break;
    case '-':
        addToken(TokenType::HyphenMinus);
        break;
    case '+':
        addToken(TokenType::PlusSign);
        break;
    case ';':
        addToken(TokenType::Semicolon);
        break;
    case '*':
        addToken(TokenType::Asterisk);
        break;

    case '!':
        addToken(match('=') ? TokenType::BangEqual : TokenType::ExclamationMark);
        break;
    case '=':
        addToken(match('=') ? TokenType::EqualEqual : TokenType::EqualsSign);
        break;
    case '<':
        addToken(match('=') ? TokenType::LessEqual : TokenType::LessThanSign);
        break;
    case '>':
        addToken(match('=') ? TokenType::GreaterEqual : TokenType::GreaterThanSign);
        break;

    case '/':
        if (match('/')) {
            // A comment goes until the end of the line.
            while (peek() != '\n' and !isAtEnd()) {
                advance();
            }
        } else {
            addToken(TokenType::Solidus);
        }
        break;
    case ' ':
        [[fallthrough]];
    case '\r':
        [[fallthrough]];
    case '\t':
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
            // TODO raft::error()
            std::cout << "Unexpected character: '" << c << "', " << line << ":" << current << std::endl;
        }
        break;
    }
}

char Scanner::advance()
{
    // TODO consider using https://github.com/nemtrif/utfcpp
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
        // TODO raft::error()
        std::cerr << "Unterminated string" << std::endl;
        return;
    }

    // The closing "
    advance();

    // Trim the surrounding quotes
    const std::string value = source.substr(start + 1, current - 1);
    addToken(TokenType::StringLiteral, value);
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

    double value = std::stod(source.substr(start, current));
    addToken(TokenType::NumberLiteral, value);
}

void Scanner::identifier()
{
    static std::map<std::string, TokenType> keywords = {
        {"and", TokenType::And},     {"class", TokenType::Class},   {"else", TokenType::Else},
        {"false", TokenType::False}, {"fun", TokenType::Fun},       {"for", TokenType::For},
        {"if", TokenType::If},       {"nil", TokenType::Nil},       {"or", TokenType::Or},
        {"print", TokenType::Print}, {"return", TokenType::Return}, {"super", TokenType::Super},
        {"this", TokenType::This},   {"true", TokenType::True},     {"var", TokenType::Var},
        {"while", TokenType::While}};

    while (isAlphaNumeric(peek())) {
        advance();
    }
    const std::string text = source.substr(start, current);
    if (auto it = keywords.find(text); it != keywords.end()) {
        addToken(it->second);
    } else {
        addToken(TokenType::Identifier);
    }
}

void Scanner::addToken(TokenType type)
{
    addToken(type, Null{});
}

void Scanner::addToken(TokenType type, Object literal)
{
    const std::string text = source.substr(start, current - start);
    Token t{type, text, literal, line};
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
