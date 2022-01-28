#pragma once

#include <vector>

#include "Statement.hpp"

namespace Lox {

class Parser
{
public:
  std::vector<Stmt> parse();

  Parser(std::vector<Token> tokens);

private:
  std::vector<Stmt> block();

  Stmt declaration();
  Stmt statement();
  Stmt varDeclaration();
  Stmt printStatement();
  Stmt expressionStatement();
  Stmt ifStatement();
  Stmt whileStatement();
  Stmt forStatement();
  Stmt function();
  Stmt returnStatement();

  Expr expression();
  Expr assignment();
  Expr equality();
  Expr comparison();
  Expr term();
  Expr factor();
  Expr unary();
  Expr primary();
  Expr orExpr();
  Expr andExpr();
  Expr call();
  Expr finishCall(Expr);

  bool match(TokenType token_type);
  bool matchOneOf(std::vector<TokenType> token_types);
  bool check(TokenType type) const;

  Token consume(TokenType token_type, std::string message);
  Token advance();

  bool isAtEnd() const;
  Token peek() const;
  Token previous() const;

private:
  std::vector<Token> tokens;
  size_t current;
};

} // namespace Lox