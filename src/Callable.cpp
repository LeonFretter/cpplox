#include <Callable.hpp>

namespace Lox {

#pragma region native_function

size_t
NativeFunction::arity() const
{
  return _arity;
}

Object
NativeFunction::call(Interpreter&, std::vector<Object> const& args) const
{
  if (fn)
    return fn(args);
  else
    return Object::null();
}

std::string
NativeFunction::toString() const
{
  return "<raw callable>";
}

std::unique_ptr<Callable>
NativeFunction::clone() const
{
  return std::make_unique<NativeFunction>(fn, _arity);
}

NativeFunction::NativeFunction(CallableFn fn, size_t in_arity)
  : fn(fn)
  , _arity(in_arity)
{}

#pragma endregion native_function

#pragma region lox_function

size_t
LoxFunction::arity() const
{
  return declaration->params().size();
}

Object
LoxFunction::call(Interpreter& interpreter,
                  std::vector<Object> const& args) const
{
  auto env = std::make_shared<Environment>(closure);
  for (auto i = 0; i < declaration->params().size(); ++i) {
    env->define(declaration->params().at(i).lexeme(), args.at(i));
  }

  try {
    interpreter.executeBlock(declaration->body(), env);
  } catch (ReturnValue& v) {
    return v.value();
  }

  return Object::null();
}

std::string
LoxFunction::toString() const
{
  return "<fn " + declaration->name().lexeme() + ">";
}

std::unique_ptr<Callable>
LoxFunction::clone() const
{
  return std::make_unique<LoxFunction>(
    std::unique_ptr<FunctionDeclarationStatement>{
      dynamic_cast<FunctionDeclarationStatement*>(
        declaration->clone().release()),
    },
    closure);
}

LoxFunction::LoxFunction(
  std::unique_ptr<FunctionDeclarationStatement>&& declaration)
  : declaration(std::move(declaration))
{}

LoxFunction::LoxFunction(
  std::unique_ptr<FunctionDeclarationStatement>&& declaration,
  SharedEnv closure)
  : declaration(std::move(declaration))
  , closure(closure)
{}

#pragma endregion // lox_function

} // namespace Lox