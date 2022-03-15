#pragma once

#include <string_view>
#include <vector>

#include "token.h"

namespace raft {

class Scanner {
public:
    explicit Scanner(std::string_view source);

    std::vector<Token> scanTokens();

private:
    bool isAtEnd();
    void scanToken();

    char advance();
    bool match(char expected);
    char peek();
    char peekNext();

    void string(char quote);
    void number();
    void identifier();

    void addToken(Token::Kind kind);
    void addToken(Token::Kind kind, object::Object literal);

    bool isDigit(char c);
    bool isAlpha(char c);
    bool isAlphaNumeric(char c);

    std::string_view substr();

    void scanError(const std::string &message);

    std::string_view source;
    std::vector<Token> tokens;

    std::size_t start = 0;
    std::size_t current = 0;
    std::size_t line = 1;
};
}  // namespace raft
