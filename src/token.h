#pragma once

#include <map>
#include <string>

#include "object.h"

namespace raft {

class Token {
public:
    enum class Type {
        Unrecognized,
#define TOKEN(kind) kind,
#include "token.def"
        EndOfFile,
        // Aliases
        Bang = ExclamationMark,
        Dot = FullStop,
        Equal = EqualsSign,
        Minus = HyphenMinus,
        Plus = PlusSign,
        Slash = Solidus,
        Star = Asterisk,
    };

    Token(Type type, std::string lexeme, object::Object literal, std::size_t line);

    std::string toString();

    Type type = Type::Unrecognized;
    std::string lexeme;
    object::Object literal;
    std::size_t line = 0;

private:
    static std::string type2str(Type type);
};

}  // namespace raft
