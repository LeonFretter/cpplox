#pragma once

#include <map>
#include <string>

#include "LoxRuntimeError.hpp"
#include "Token.hpp"

namespace Lox {

class Environment
{
public:
  void define(std::string name, Object value)
  {
    if (!values.contains(name)) {
      values.emplace(name, value);
    } else {
      values.at(name) = value;
    }
  }

  void assign(Token name, Object value)
  {
    if (!values.contains(name.lexeme())) {
      throw LoxRuntimeError{ name,
                             "Undefined variable '" + name.lexeme() + "'" };
    } else {
      values.at(name.lexeme()) = value;
    }
  }
  //   TODO: consider if ref is needed
  Object get(Token name) const
  {
    try {
      return values.at(name.lexeme());
    } catch (std::runtime_error err) {
      throw LoxRuntimeError{ name, "Undefined variable " + name.lexeme() };
    }
  }

private:
  std::map<std::string, Object> values;
};

} // namespace Lox