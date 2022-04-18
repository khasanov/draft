#pragma once

#include <string>

#include "object.h"

namespace raft {

class Token {
public:
    enum class Kind {
        Unrecognized,
#define TOKEN(kind) kind,
#include "token.def"
        EndOfFile,
    };

    Token(Kind kind, std::string lexeme, object::Object literal, std::size_t line);

    std::string toString();

    const Kind kind = Kind::Unrecognized;
    std::string lexeme;
    object::Object literal;
    std::size_t line = 0;
};

}  // namespace raft
