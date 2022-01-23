#pragma once

#include <vector>

#include "Expression.hpp"

namespace Lox {

class Parser
{
public:
  Expr parse() { return expression(); }

  Parser(std::vector<Token> tokens)
    : tokens(tokens)
    , current(0UL)
  {}

private:
  Expr expression() { return equality(); }

  Expr equality()
  {
    auto expr = comparison();

    while (matchOneOf({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
      auto op = previous();
      auto rhs = comparison();
      // previous expression becomes lhs
      expr =
        std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
    }

    return expr;
  }

  Expr comparison()
  {
    auto expr = term();

    while (matchOneOf({ TokenType::GREATER,
                        TokenType::GREATER_EQUAL,
                        TokenType::LESS,
                        TokenType::LESS_EQUAL })) {
      auto op = previous();
      auto rhs = term();
      expr =
        std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
    }

    return expr;
  }

  Expr term()
  {
    auto expr = factor();

    while (matchOneOf({ TokenType::MINUS, TokenType::PLUS })) {
      auto op = previous();
      auto rhs = factor();
      expr =
        std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
    }

    return expr;
  }

  Expr factor()
  {
    auto expr = unary();

    while (matchOneOf({ TokenType::SLASH, TokenType::STAR })) {
      auto op = previous();
      auto rhs = unary();
      expr =
        std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
    }

    return expr;
  }

  Expr unary()
  {
    if (matchOneOf({ TokenType::BANG, TokenType::MINUS })) {
      auto op = previous();
      auto rhs = unary();
      return std::make_unique<UnaryExpression>(op, std::move(rhs));
    }

    return primary();
  }

  Expr primary()
  {
    if (match(TokenType::FALSE)) {
      return std::make_unique<LiteralExpression>(Object{ false });
    }
    if (match(TokenType::TRUE)) {
      return std::make_unique<LiteralExpression>(Object{ true });
    }
    if (match(TokenType::NIL)) {
      return std::make_unique<LiteralExpression>(Object{});
    }
    if (matchOneOf({ TokenType::NUMBER, TokenType::STRING })) {
      return std::make_unique<LiteralExpression>(previous().literal());
    }

    if (match(TokenType::LEFT_PAREN)) {
      auto expr = expression();
      consume(TokenType::RIGHT_PAREN, "Unterminated parentheses");
      return std::make_unique<GroupingExpression>(std::move(expr));
    }

    throw std::runtime_error("Expected an expression.");
  }

  bool match(TokenType token_type) { return matchOneOf({ token_type }); }
  bool matchOneOf(std::vector<TokenType> token_types)
  {
    for (auto t : token_types) {
      if (check(t)) {
        advance();
        return true;
      }
    }
    return false;
  }

  bool check(TokenType type) const { return peek().type() == type; }

  Token consume(TokenType token_type, std::string message)
  {
    if (check(token_type))
      return advance();
    else
      throw std::runtime_error(message);
  }

  Token advance()
  {
    if (!isAtEnd()) {
      return tokens.at(current++);
    }
    return Token{ TokenType::END_OF_FILE, "" };
  }

  bool isAtEnd() const { return peek().type() == TokenType::END_OF_FILE; }
  Token peek() const { return tokens.at(current); }
  Token previous() const { return tokens.at(current - 1UL); }

private:
  std::vector<Token> tokens;
  size_t current;
};

} // namespace Lox