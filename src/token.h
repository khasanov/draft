#pragma once

#include <string>

#include "object.h"

namespace draft {

using Lexeme = std::string;

class Token {
public:
    enum class Kind {
        Unrecognized,
#define TOKEN(kind) kind,
#include "token.def"
        EndOfFile,
    };

    Token(Kind kind, Lexeme lexeme, object::Object literal, std::size_t line);

    std::string toString();

    const Kind kind = Kind::Unrecognized;
    Lexeme lexeme;
    object::Object literal;
    std::size_t line = 0;
};

}  // namespace draft
