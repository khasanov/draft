# Raft programming language

My experiments in cRAFTing a toy programming language.

## Readings

[Crafting Interpreters](craftinginterpreters.com)

## Grammar

```
program    :: statement* EOF
statement  :: exprStmt | printStmt
exprStmt   :: expresson ";"
printStmt  :: "print" expression ";"

expression :: equality
equality   :: comparison ( ("!=" | "==" ) comparision)*
comparison :: term ( ( ">" | ">=" | "<" | "<=" ) term )*
term       :: factor ( ("-" | "+" ) factor )*
factor     :: unary ( ( "/" | "*" ) unary )*
unary      :: ( "!" "-" ) unary | primary
primary    :: NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")"
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
