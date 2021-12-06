#pragma once

#include <map>
#include <string>

#include "object.h"

namespace raft {

class Token {
public:
    enum class Type {
        Unrecognized,

        // Single-character tokens. Names are taken from Unicode Standard
        Null,                    // \0 U+0000
        StartOfHeading,          // U+0001
        StartOfText,             // U+0002
        EndOfText,               // U+0003
        EndOfTransmission,       // U+0004
        Enquiry,                 // U+0005
        Achnowledge,             // U+0006
        Bell,                    // \a U+0007
        Backspace,               // \b U+0008
        HorizontalTabulation,    // \t U+0009
        NewLine,                 // \n U+000A
        VerticalTabulation,      // \v U+000B
        FormFeed,                // \f U+000C
        CarriageReturn,          // \r U+000D
        ShiftOut,                // U+000E
        ShiftIn,                 // U+000F
        DataLinkEscape,          // U+0010
        DeviceControlOne,        // U+0011
        DeviceControlTwo,        // U+0012
        DeviceControlThree,      // U+0013
        DeviceControlFour,       // U+0014
        NegativeAcknowledge,     // U+0015
        SynchronousIdle,         // U+0016
        EndOfTransmissionBlock,  // U+0017
        Cancel,                  // U+0018
        EndOfMedium,             // U+0019
        Substitute,              // U+001A
        Escape,                  // \e U+001B
        FileSeparator,           // U+001C
        GroupSeparator,          // U+001D
        RecordSeparator,         // U+001E
        UnitSeparator,           // U+001F
        Space,                   //   U+0020
        ExclamationMark,         // ! U+0021
        QuotationMark,           // " U+0022
        NumberSign,              // # U+0023
        DollarSign,              // $ U+0024
        PercentSign,             // % U+0025
        Ampersand,               // & U+0026
        Apostrophe,              // ' U+0027
        LeftParenthesis,         // ( U+0028
        RightParenthesis,        // ) U+0029
        Asterisk,                // * U+002A
        PlusSign,                // + U+002B
        Comma,                   // , U+002C
        HyphenMinus,             // - U+002D
        FullStop,                // . U+002E
        Solidus,                 // / U+002F
        // DigitZero, // 0 U+0030
        // ...
        // DigitNine, // 9 U+0039
        Colon,            // : U+003A
        Semicolon,        // ; U+003B
        LessThanSign,     // < U+003C
        EqualsSign,       // = U+003D
        GreaterThanSign,  // > U+003E
        QuestionMark,     // ? U+003F
        CommercialAt,     // @ U+0040
        // LatinCapitalLetterA // A U+0041
        // ...
        // LatinCapitalLetterZ // Z U+005A
        LeftSquareBracket,   // [ U+005B
        ReverseSolidus,      // \ U+005C
        RightSquareBracket,  // ] U+005D
        CircumflexAccent,    // ^ U+005E
        LowLine,             // _ U+005F
        GraveAccent,         // ` U+0060
        // LatinSmallLetterA // a U+0061
        // ...
        // LatinSmallLetterZ // z U+007A
        LeftCurlyBracket,   // { U+007B
        VerticalLine,       // | U+007C
        RightCurlyBracket,  // } U+007D
        Tilde,              // ~ U+007E

        // Two character tokens
        BangEqual,     // !=
        EqualEqual,    // ==
        GreaterEqual,  // >=
        LessEqual,     // <=

        // Atoms
        Identifier,     // abc123
        NumberLiteral,  // 123
        StringLiteral,  // "123"

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

    static std::map<Type, std::string> typeNames;
};

}  // namespace raft
