#pragma once

#include <memory>

#include "Interpreter.hpp"

namespace Lox {

using CallableFn = std::function<Object(std::vector<Object> const&)>;

class Callable
{
public:
  virtual size_t arity() const = 0;
  virtual Object call(Interpreter&, std::vector<Object> const& args) const = 0;
  virtual std::string toString() const = 0;
  virtual std::unique_ptr<Callable> clone() const = 0;

  Callable() = default;
  Callable(Callable const&) = delete;
  Callable& operator=(Callable const&) = delete;
  virtual ~Callable() = default;

private:
};

class NativeFunction : public Callable
{
public:
  virtual size_t arity() const override;

  virtual Object call(Interpreter&,
                      std::vector<Object> const& args) const override;

  virtual std::string toString() const override;
  virtual std::unique_ptr<Callable> clone() const override;

  NativeFunction() = default;
  explicit NativeFunction(CallableFn fn, size_t in_arity);

private:
  CallableFn fn;
  size_t _arity;
};

class LoxFunction : public Callable
{
public:
  virtual size_t arity() const override;
  virtual Object call(Interpreter& interpreter,
                      std::vector<Object> const& args) const override;
  virtual std::string toString() const override;
  virtual std::unique_ptr<Callable> clone() const override;

  LoxFunction(std::unique_ptr<FunctionDeclarationStatement>&& declaration);
  LoxFunction(std::unique_ptr<FunctionDeclarationStatement>&& declaration,
              SharedEnv closure);

private:
  std::unique_ptr<FunctionDeclarationStatement> declaration;
  SharedEnv closure;
};

class ReturnValue : public std::exception
{
public:
  Object const& value() const { return v; }

  ReturnValue()
    : v(Object::null())
  {}
  ReturnValue(Object v)
    : v(v)
  {}

private:
  Object v;
};

} // namespace Lox