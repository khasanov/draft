#include <iomanip>
#include <iostream>

#include "raft.h"
#include "scanner.h"

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

    Token eof{Token::Kind::EndOfFile, "", object::Null{}, line};
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
        addToken(Token::Kind::Dot);
        break;
    case '-':
        addToken(Token::Kind::Minus);
        break;
    case '+':
        addToken(Token::Kind::Plus);
        break;
    case ';':
        addToken(Token::Kind::Semicolon);
        break;
    case '*':
        addToken(Token::Kind::Star);
        break;
    case '!':
        addToken(match('=') ? Token::Kind::BangEqual : Token::Kind::Bang);
        break;
    case '=':
        addToken(match('=') ? Token::Kind::EqualEqual : Token::Kind::Equal);
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
            addToken(Token::Kind::Slash);
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

    std::string_view value = substr();
    // Trim the surrounding quotes
    value.remove_prefix(1);
    value.remove_suffix(1);
    addToken(Token::Kind::StringLiteral, object::String{value});
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

    double value = std::stod(std::string{substr()});
    addToken(Token::Kind::NumberLiteral, object::Number{value});
}

void Scanner::identifier()
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

void Scanner::addToken(Token::Kind kind)
{
    addToken(kind, object::Null{});
}

void Scanner::addToken(Token::Kind kind, object::Object literal)
{
    Token t{kind, std::string{substr()}, literal, line};
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

std::string_view Scanner::substr()
{
    std::size_t pos = start;
    std::size_t count = current - pos;
    return source.substr(pos, count);
}

}  // namespace raft
