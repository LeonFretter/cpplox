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

enum class ObjectType
{
  BOOLEAN,
  NIL,
  NUMBER,
  STRING
};

class Object
{
public:
  std::string const& string() const { return str.value(); }
  double number() const { return num.value(); }
  bool boolean() const { return _boolean.value(); }

  bool isString() const noexcept { return str.has_value(); }
  bool isNumber() const noexcept { return num.has_value(); }
  bool isBoolean() const noexcept { return _boolean.has_value(); }
  bool isNull() const noexcept { return !operator bool(); }

  ObjectType type() const noexcept { return _type; }

  operator bool() const noexcept
  {
    return str.has_value() || num.has_value() || _boolean.has_value();
  }

  Object()
    : str()
    , num()
    , _boolean()
    , _type(ObjectType::NIL)
  {}
  explicit Object(std::string str)
    : str(str)
    , num()
    , _boolean()
    , _type(ObjectType::STRING)
  {}
  explicit Object(double num)
    : str()
    , num(num)
    , _boolean()
    , _type(ObjectType::NUMBER)
  {}
  explicit Object(bool boolean)
    : str()
    , num()
    , _boolean(boolean)
    , _type(ObjectType::BOOLEAN)
  {}

  static Object null() { return Object{}; }

private:
  std::optional<std::string> str;
  std::optional<double> num;
  std::optional<bool> _boolean;
  ObjectType _type;
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