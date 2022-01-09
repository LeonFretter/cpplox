#pragma once

#include <iostream>
#include <vector>

#include "Token.hpp"

namespace Lox {

class Scanner
{
public:
  std::vector<Token> scanTokens();

  Scanner(size_t src_sz, char const* src);
  Scanner(std::vector<char> const& src);
  Scanner(std::string const& src);

private:
  void scanToken();
  void addToken(TokenType);
  void addToken(TokenType, Object literal);
  void addString();
  void addNumber();
  void addIdentifier();

  bool match(char expected);
  char advance();
  char peek(size_t offset = 0UL) const;

  bool isAtEnd(size_t offset = 0UL) const
  {
    return (current + offset) >= src_sz;
  }

  static TokenType oneCharTokenType(char c);
  static bool isWhitespace(char c);

  static constexpr bool isDigit(char c) { return c >= '0' && c <= '9'; }

  static constexpr bool isAlpha(char c)
  {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  static constexpr bool isAlphaNumeric(char c)
  {
    return isAlpha(c) || isDigit(c);
  }

private:
  size_t src_sz;
  char const* src;
  std::vector<Token> tokens;
  size_t start;
  size_t current;
};

} // namespace Lox