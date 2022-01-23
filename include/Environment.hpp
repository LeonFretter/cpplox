#pragma once

#include <map>
#include <memory>
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
      if (parent) {
        parent->assign(name, value);
      } else {
        throw LoxRuntimeError{ name,
                               "Undefined variable '" + name.lexeme() + "'" };
      }
    } else {
      values.at(name.lexeme()) = value;
    }
  }
  //   TODO: consider if ref is needed
  Object get(Token name) const
  {
    if (values.contains(name.lexeme())) {
      return values.at(name.lexeme());
    } else if (parent) {
      return parent->get(name);
    } else {
      throw LoxRuntimeError{ name, "Undefined variable " + name.lexeme() };
    }
  }

  Environment() = default;
  Environment(Environment* parent)
    : parent(parent)
    , values()
  {}

private:
  Environment* parent;
  std::map<std::string, Object> values;
};

using Env = std::unique_ptr<Environment>;

} // namespace Lox