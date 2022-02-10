#pragma once

#include <memory>
#include <optional>
#include <string>

#include <functional>

namespace Lox {

enum class ObjectType
{
  BOOLEAN,
  CALLABLE,
  CLASS,
  NIL,
  NUMBER,
  STRING,
};

class Callable;
class LoxClass;

class Object
{
public:
  std::string toString() const;

  std::string const& string() const;
  double number() const;
  bool boolean() const;
  Callable& callable() const;
  LoxClass& klass() const;

  bool isString() const noexcept;
  bool isNumber() const noexcept;
  bool isBoolean() const noexcept;
  bool isNull() const noexcept;
  bool isCallable() const noexcept;
  bool isClass() const noexcept;

  ObjectType type() const noexcept;

  operator bool() const noexcept;

  Object();
  explicit Object(std::string str);
  explicit Object(double num);
  explicit Object(bool boolean);
  explicit Object(std::unique_ptr<Callable> in_callable);
  explicit Object(std::unique_ptr<LoxClass> in_class);
  Object(Object const& orig);
  Object& operator=(Object const& orig);
  ~Object();

  static Object null();

private:
  std::optional<std::string> str;
  std::optional<double> num;
  std::optional<bool> _boolean;
  std::unique_ptr<Callable> _callable;
  std::unique_ptr<LoxClass> _class;
  ObjectType _type;
};

} // namespace Lox