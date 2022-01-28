#include <Environment.hpp>

namespace Lox {

void
Environment::define(std::string name, Object value)
{
  if (!values.contains(name)) {
    values.emplace(name, value);
  } else {
    values.at(name) = value;
  }
}

void
Environment::assign(Token name, Object value)
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

Object
Environment::get(Token name) const
{
  return get(name.lexeme());
}

Environment::Environment()
  : parent(nullptr)
  , values()
{}
Environment::Environment(SharedEnv parent)
  : parent(parent)
  , values()
{}
Environment::Environment(SharedEnv parent, std::map<std::string, Object> values)
  : parent(parent)
  , values(values)
{}

Object
Environment::get(std::string name) const
{
  if (values.contains(name)) {
    return values.at(name);
  } else if (parent) {
    return parent->get(name);
  } else {
    throw LoxRuntimeError{ name, "Undefined variable " + name };
  }
}

} // namespace Lox