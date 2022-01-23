#pragma once

#include "Token.hpp"

namespace Lox {

class LoxRuntimeError : public std::runtime_error
{
public:
  LoxRuntimeError(Token token, std::string err)
    : std::runtime_error(err)
    , token(token)
  {}

private:
  Token token;
};

} // namespace Lox