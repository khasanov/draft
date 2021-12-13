# Raft programming language

My experiments in cRAFTing a toy programming language.

## Readings

[Crafting Interpreters](craftinginterpreters.com)

## Grammar

```
program     :: declaration* EOF
declaration :: varDecl | statement
statement   :: exprStmt | ifStmt | printStmt | block
exprStmt    :: expresson ";"
ifStmt      :: "if" "(" expression ")" statement ( "else" statement )?
printStmt   :: "print" expression ";"
block       :: "{" declaration* "}"
varDecl     :: "var" IDENTIFIER ( "=" expression )? ";"

expression  :: assignment
assignment  :: IDENTIFIER "=" assignment | equality
equality    :: comparison ( ("!=" | "==" ) comparision)*
comparison  :: term ( ( ">" | ">=" | "<" | "<=" ) term )*
term        :: factor ( ("-" | "+" ) factor )*
factor      :: unary ( ( "/" | "*" ) unary )*
unary       :: ( "!" "-" ) unary | primary
primary     :: NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER
```

## Ast representation

```
Expr{}                      for expression
Lit{value}                  for literals: NUMBER, STRING, "true", "false", "nil"
UnOp{op, Expr{}}            for unary
BinOp{op, Expr{}, Expr{}}   for binary
{Expr{}}                    for grouping: "(" expression ")"

ExprStmt{Expr{}}            for expression statement
PrintStmt{Expr{}}           for print statement
```
