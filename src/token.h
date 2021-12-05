#pragma once

#include <string>
#include <variant>

namespace raft {

enum TokenType {
    Unrecognized,

    // Single-character tokens. Names are taken from Unicode Standard
    Space,             //   U+0020
    ExclamationMark,   // ! U+0021
    QuotationMark,     // " U+0022
    NumberSign,        // # U+0023
    DollarSign,        // $ U+0024
    PercentSign,       // % U+0025
    Ampersand,         // & U+0026
    Apostrophe,        // ' U+0027
    LeftParenthesis,   // ( U+0028
    RightParenthesis,  // ) U+0029
    Asterisk,          // * U+002A
    PlusSign,          // + U+002B
    Comma,             // , U+002C
    HyphenMinus,       // - U+002D
    FullStop,          // . U+002E
    Solidus,           // / U+002F

    // DigitZero // 0 U+0030
    // DigitNine // 9 U+0039

    Colon,            // : U+003A
    Semicolon,        // ; U+003B
    LessThanSign,     // < U+003C
    EqualsSign,       // = U+003D
    GreaterThanSign,  // > U+003E
    QuestionMark,     // ? U+003F
    CommercialAt,     // @ U+0040

    // LatinCapitalLetterA // A U+0041
    // LatinCapitalLetterZ // Z U+005A

    LeftSquareBracket,   // [ U+005B
    ReverseSolidus,      // \ U+005C
    RightSquareBracket,  // ] U+005D
    CircumflexAccent,    // ^ U+005E
    LowLine,             // _ U+005F
    GraveAccent,         // ` U+0060

    // LatinSmallLetterA // a U+0061
    // LatinSmallLetterZ // z U+007A

    LeftCurlyBracket,   // { U+007B
    VerticalLine,       // | U+007C
    RightCurlyBracket,  // } U+007D
    Tilde,              // ~ U+007E

    // Two character tokens
    BangEqual,     // !=
    EqualEqual,    // ==
    LessEqual,     // <=
    GreaterEqual,  // >=

    // Literals
    Identifier,     // abc123
    StringLiteral,  // "123"
    NumberLiteral,  // 123

    // Keywords
    And,     // and
    Class,   // class
    Else,    // else
    False,   // false
    Fun,     // fun
    For,     // for
    If,      // if
    Nil,     // nil
    Or,      // or
    Print,   // print
    Return,  // return
    Super,   // super
    This,    // this
    True,    // true
    Var,     // var
    While,   // while

    EndOfFile
};

using Null = std::monostate;
using String = std::string;
using Number = double;
using Object = std::variant<Null, String, Number>;

class Token {
public:
    Token(TokenType type, std::string lexeme, Object literal, std::size_t line);

    std::string toString();

    TokenType type = TokenType::Unrecognized;
    std::string lexeme;
    Object literal;
    std::size_t line = std::numeric_limits<std::size_t>::max();

private:
    static std::string type2str(TokenType type);
    static std::string obj2str(const Object &obj);
};

}  // namespace raft
