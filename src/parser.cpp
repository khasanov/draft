#include "parser.h"

#include "raft.h"

namespace raft {

RuntimeError::RuntimeError(const Token &token, const std::string &message)
    : std::runtime_error{message}
    , token{token}
{
}

/*
A recursive descent parser is a literal translation of the grammar's rules straight into imperative
code. Each rule becomes a function. The body of the rule translates to code roughtly like:

Grammar notation  Code representation
Terminal          Code to match and consule a token
Nonterminal       Call to that rule's function
|                 if or switch statement
* or +            while of for loop
?                 if statement
*/
Parser::Parser(const std::vector<Token> &tokens)
    : tokens{tokens}
{
}

Expr *Parser::parse()
{
    try {
        return expression();
    } catch (const RuntimeError &ex) {
        Raft::error(ex.token.line, ex.what());
        synchronize();
        return nullptr;
    }
}

// expression :: equality
Expr *Parser::expression()
{
    return equality();
}

// equality :: comparison ( ( "!=" | "==" ) comparision )*
Expr *Parser::equality()
{
    Expr *expr = comparison();
    while (match(Token::Type::BangEqual, Token::Type::EqualEqual)) {
        Token op = previous();
        Expr *right = comparison();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )*
Expr *Parser::comparison()
{
    Expr *expr = term();
    while (match(Token::Type::GreaterThanSign, Token::Type::GreaterEqual, Token::Type::LessThanSign,
                 Token::Type::LessEqual)) {
        Token op = previous();
        Expr *right = term();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// term :: factor ( ("-" | "+" ) factor )*
Expr *Parser::term()
{
    Expr *expr = factor();
    while (match(Token::Type::Minus, Token::Type::Plus)) {
        Token op = previous();
        Expr *right = factor();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// factor :: unary ( ( "/" | "*" ) unary )*
Expr *Parser::factor()
{
    Expr *expr = unary();
    while (match(Token::Type::Slash, Token::Type::Star)) {
        Token op = previous();
        Expr *right = unary();
        expr = makeAstNode<Binary>(expr, op, right);
    }
    return expr;
}

// unary :: ( "!" "-" ) unary | primary
Expr *Parser::unary()
{
    if (match(Token::Type::Bang, Token::Type::Minus)) {
        Token op = previous();
        Expr *right = unary();
        return makeAstNode<Unary>(op, right);
    }
    return primary();
}

// primary :: NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
Expr *Parser::primary()
{
    if (match(Token::Type::False)) {
        return makeAstNode<Literal>(object::Boolean{false});
    }
    if (match(Token::Type::True)) {
        return makeAstNode<Literal>(object::Boolean{true});
    }
    if (match(Token::Type::Nil)) {
        return makeAstNode<Literal>(object::Null{});
    }
    if (match(Token::Type::NumberLiteral, Token::Type::StringLiteral)) {
        return makeAstNode<Literal>(previous().literal);
    }

    if (match(Token::Type::LeftParenthesis)) {
        Expr *expr = expression();
        consume(Token::Type::RightParenthesis, "Exprect ')' after expression");
        return makeAstNode<Grouping>(expr);
    }

    throw RuntimeError{peek(), "Expect expression"};
}

bool Parser::isAtEnd()
{
    return peek().type == Token::Type::EndOfFile;
}

Token Parser::peek()
{
    return tokens.at(current);
}

Token Parser::previous()
{
    return tokens.at(current - 1);
}

Token Parser::advance()
{
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::check(Token::Type type)
{
    if (isAtEnd()) {
        return false;
    }
    return peek().type == type;
}

Token Parser::consume(Token::Type type, const std::string &message)
{
    if (check(type)) {
        return advance();
    }
    throw RuntimeError{peek(), message};
}

void Parser::synchronize()
{
    advance();

    while (!isAtEnd()) {
        if (previous().type == Token::Type::Semicolon) {
            return;
        }

        switch (peek().type) {
        case Token::Type::Class:
            [[fallthrough]];
        case Token::Type::Fun:
            [[fallthrough]];
        case Token::Type::Var:
            [[fallthrough]];
        case Token::Type::For:
            [[fallthrough]];
        case Token::Type::If:
            [[fallthrough]];
        case Token::Type::While:
            [[fallthrough]];
        case Token::Type::Print:
            [[fallthrough]];
        case Token::Type::Return:
            return;
        default:
            break;
        }

        advance();
    }
}

}  // namespace raft
