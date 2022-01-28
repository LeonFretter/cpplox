#pragma once

#include <map>
#include <memory>
#include <string>

#include "LoxRuntimeError.hpp"
#include "Token.hpp"

namespace Lox {

class Environment;

using Env = std::unique_ptr<Environment>;
using SharedEnv = std::shared_ptr<Environment>;

class Environment
{
public:
  void define(std::string name, Object value);

  void assign(Token name, Object value);
  //   TODO: consider if ref is needed
  Object get(Token name) const;

  Environment();
  Environment(SharedEnv parent);
  Environment(SharedEnv parent, std::map<std::string, Object> values);

private:
  Object get(std::string name) const;

private:
  SharedEnv parent;
  std::map<std::string, Object> values;
};

} // namespace Lox