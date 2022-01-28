#pragma once

#include "Token.hpp"

namespace Lox {

class LoxRuntimeError : public std::runtime_error
{
public:
  LoxRuntimeError(Token token, std::string err)
    : std::runtime_error(err)
    , lexeme(token.lexeme())
  {}
  LoxRuntimeError(std::string lexeme, std::string err)
    : std::runtime_error(err)
    , lexeme(lexeme)
  {}

private:
  std::string lexeme;
};

} // namespace Lox