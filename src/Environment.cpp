#include <Environment.hpp>

namespace Lox {

void
Environment::define(std::string name, Object value)
{
  if (!values.contains(name)) {
    values.emplace(name, value);
  } else {
    throw std::runtime_error("Trying to redefine");
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

void
Environment::assignAt(size_t depth, Token name, Object value)
{
  ancestor(depth).assign(name, value);
}

Object
Environment::get(Token name) const
{
  return get(name.lexeme());
}

Object
Environment::getAt(size_t depth, Token name) const
{
  return ancestor(depth).get(name);
}

std::string
Environment::toString() const
{
  auto res = std::string{};
  for (auto& [k, v] : values) {
    res += "{k: " + k + ", v: " + v.toString() + "},";
  }
  return res;
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

Environment const&
Environment::ancestor(size_t distance) const
{
  auto* env = this;
  for (auto i = 0; i < distance; ++i) {
    env = env->parent.get();
  }

  return *env;
}

Environment&
Environment::ancestor(size_t distance)
{
  return const_cast<Environment&>(std::as_const(*this).ancestor(distance));
}

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