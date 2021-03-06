#include <iomanip>
#include <iostream>

#include "driver.h"
#include "lexer.h"

namespace draft {

Lexer::Lexer(std::string_view source)
    : source{source}
{
}

std::vector<Token> Lexer::scanTokens()
{
    while (!isAtEnd()) {
        // We are at the beginning of the next lexeme
        start = current;
        scanToken();
    }

    Token eof{Token::Kind::EndOfFile, "", object::Null{}, line};
    tokens.emplace_back(std::move(eof));
    return tokens;
}

bool Lexer::isAtEnd()
{
    return current >= source.length();
}

void Lexer::scanToken()
{
    char c = advance();
    switch (c) {
    case '(':
        addToken(Token::Kind::LeftParenthesis);
        break;
    case ')':
        addToken(Token::Kind::RightParenthesis);
        break;
    case '{':
        addToken(Token::Kind::LeftCurlyBracket);
        break;
    case '}':
        addToken(Token::Kind::RightCurlyBracket);
        break;
    case ',':
        addToken(Token::Kind::Comma);
        break;
    case '.':
        addToken(Token::Kind::FullStop);
        break;
    case '-':
        addToken(Token::Kind::HyphenMinus);
        break;
    case '+':
        addToken(Token::Kind::PlusSign);
        break;
    case ';':
        addToken(Token::Kind::Semicolon);
        break;
    case '*':
        addToken(Token::Kind::Asterisk);
        break;
    case '!':
        addToken(match('=') ? Token::Kind::ExclaimEqual : Token::Kind::ExclamationMark);
        break;
    case '=':
        addToken(match('=') ? Token::Kind::EqualEqual : Token::Kind::EqualsSign);
        break;
    case '<':
        addToken(match('=') ? Token::Kind::LessEqual : Token::Kind::LessThanSign);
        break;
    case '>':
        addToken(match('=') ? Token::Kind::GreaterEqual : Token::Kind::GreaterThanSign);
        break;
    case '/':
        if (match('/')) {
            // A comment goes until the end of the line
            while (peek() != '\n' and !isAtEnd()) {
                advance();
            }
        } else {
            addToken(Token::Kind::Solidus);
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
        string('"');
        break;
    case '\'':
        string('\'');
        break;
    default:
        if (isDigit(c)) {
            number();
        } else if (isAlpha(c)) {
            identifier();
        } else {
            std::stringstream ss;
            ss << "Unexpected character '" << c << "' at column " << current;
            scanError(ss.str());
        }
        break;
    }
}

char Lexer::advance()
{
    return source.at(current++);
}

bool Lexer::match(char expected)
{
    if (isAtEnd()) {
        return false;
    }
    if (source.at(current) != expected) {
        return false;
    }
    current++;
    return true;
}

char Lexer::peek()
{
    if (isAtEnd()) {
        return '\0';
    }
    return source.at(current);
}

char Lexer::peekNext()
{
    if (current + 1 >= source.length()) {
        return '\0';
    }
    return source.at(current + 1);
}

void Lexer::string(char quote)
{
    while (peek() != quote and not isAtEnd()) {
        if (peek() == '\n') {
            line++;
        }
        advance();
    }

    if (isAtEnd()) {
        scanError("Unterminated string");
        return;
    }

    // The closing quote
    advance();

    std::string_view value = substr();
    // Trim the surrounding quotes
    value.remove_prefix(1);
    value.remove_suffix(1);
    addToken(Token::Kind::StringLiteral, object::String{value});
}

void Lexer::number()
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

    double value = std::stod(std::string{substr()});
    addToken(Token::Kind::NumberLiteral, object::Number{value});
}

void Lexer::identifier()
{
    while (isAlphaNumeric(peek())) {
        advance();
    }

    auto maybeKeyword = [](std::string_view sv) {
#define KEYWORD(kind, name)       \
    if (name == sv) {             \
        return Token::Kind::kind; \
    }
#include "token.def"
        return Token::Kind::Identifier;
    };

    addToken(maybeKeyword(substr()));
}

void Lexer::addToken(Token::Kind kind)
{
    addToken(kind, object::Null{});
}

void Lexer::addToken(Token::Kind kind, object::Object literal)
{
    Token t{kind, std::string{substr()}, literal, line};
    tokens.emplace_back(std::move(t));
}

bool Lexer::isDigit(char c)
{
    return c >= '0' and c <= '9';
}

bool Lexer::isAlpha(char c)
{
    return (c >= 'a' and c <= 'z') or (c >= 'A' and c <= 'Z') or c == '_';
}

bool Lexer::isAlphaNumeric(char c)
{
    return isAlpha(c) or isDigit(c);
}

std::string_view Lexer::substr()
{
    std::size_t pos = start;
    std::size_t count = current - pos;
    return source.substr(pos, count);
}

void Lexer::scanError(const std::string &message)
{
    Driver::error(line, message);
}

}  // namespace draft
