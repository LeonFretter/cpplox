#pragma once

#include <iostream>
#include <optional>
#include <string>

namespace Lox {

enum class TokenType
{
  // One character tokens
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  COMMA,
  DOT,
  MINUS,
  PLUS,
  SEMICOLON,
  SLASH,
  STAR,
  // One or two character tokens
  BANG,
  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  GREATER,
  GREATER_EQUAL,
  LESS,
  LESS_EQUAL,
  // Literals
  IDENTIFIER,
  STRING,
  NUMBER,
  // Keywords
  AND,
  CLASS,
  ELSE,
  FALSE,
  FUN,
  FOR,
  IF,
  NIL,
  OR,
  PRINT,
  RETURN,
  SUPER,
  THIS,
  TRUE,
  VAR,
  WHILE,
  END_OF_FILE
};

class Object
{
public:
  std::string const& string() const { return str.value(); }
  double number() const { return num.value(); }

  bool isString() const noexcept { return str.has_value(); }
  bool isNumber() const noexcept { return num.has_value(); }

  operator bool() const noexcept { return str.has_value() || num.has_value(); }

  Object()
    : str()
    , num()
  {}
  explicit Object(std::string str)
    : str(str)
    , num()
  {}
  explicit Object(double num)
    : str()
    , num(num)
  {}

private:
  std::optional<std::string> str;
  std::optional<double> num;
};

class Token
{
public:
  friend std::ostream& operator<<(std::ostream& stream, Token const& token)
  {
    stream << token._lexeme;
    return stream;
  }

  TokenType type() const noexcept { return _type; }
  std::string const& lexeme() const noexcept { return _lexeme; }
  Object const& literal() const noexcept { return _literal; }

  Token(TokenType in_type, std::string in_lexeme)
    : _type(in_type)
    , _lexeme(in_lexeme)
    , _literal()
  {}
  Token(TokenType in_type, std::string in_lexeme, Object in_literal)
    : _type(in_type)
    , _lexeme(in_lexeme)
    , _literal(in_literal)
  {}

private:
  TokenType _type;
  std::string _lexeme;
  Object _literal;
};

} // namespace Lox