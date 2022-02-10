#include <Callable.hpp>
#include <LoxClass.hpp>
#include <Object.hpp>

namespace Lox {

std::string
Object::toString() const
{
  if (isNull())
    return "nil";
  else if (isNumber()) {
    return std::to_string(number());
  } else if (isBoolean()) {
    return boolean() == true ? "true" : "false";
  } else if (isCallable()) {
    return callable().toString();
  } else if (isClass()) {
    return klass().toString();
  } else {
    // string assumed
    return string();
  }
}

std::string const&
Object::string() const
{
  return str.value();
}

double
Object::number() const
{
  return num.value();
}

bool
Object::boolean() const
{
  return _boolean.value();
}

Callable&
Object::callable() const
{
  return *_callable;
}

LoxClass&
Object::klass() const
{
  return *_class;
}

bool
Object::isString() const noexcept
{
  return str.has_value();
}

bool
Object::isNumber() const noexcept
{
  return num.has_value();
}

bool
Object::isBoolean() const noexcept
{
  return _boolean.has_value();
}

bool
Object::isNull() const noexcept
{
  return !operator bool();
}

bool
Object::isCallable() const noexcept
{
  return _callable.operator bool();
}

bool
Object::isClass() const noexcept
{
  return _class.operator bool();
}

ObjectType
Object::type() const noexcept
{
  return _type;
}

Object::operator bool() const noexcept
{
  return str.has_value() || num.has_value() || _boolean.has_value() ||
         _callable.operator bool() || _class.operator bool();
}

Object::Object()
  : str()
  , num()
  , _boolean()
  , _callable()
  , _class()
  , _type(ObjectType::NIL)
{}

Object::Object(std::string str)
  : str(str)
  , num()
  , _boolean()
  , _callable()
  , _class()
  , _type(ObjectType::STRING)
{}

Object::Object(double num)
  : str()
  , num(num)
  , _boolean()
  , _callable()
  , _class()
  , _type(ObjectType::NUMBER)
{}

Object::Object(bool boolean)
  : str()
  , num()
  , _boolean(boolean)
  , _callable()
  , _class()
  , _type(ObjectType::BOOLEAN)
{}

Object::Object(std::unique_ptr<Callable> in_callable)
  : str()
  , num()
  , _boolean()
  , _callable(std::move(in_callable))
  , _class()
  , _type(ObjectType::CALLABLE)
{}

Object::Object(std::unique_ptr<LoxClass> in_class)
  : str()
  , num()
  , _boolean()
  , _callable()
  , _class(std::move(in_class))
  , _type(ObjectType::CLASS)
{}

Object::Object(Object const& orig)
  : str(orig.str)
  , num(orig.num)
  , _boolean(orig._boolean)
  , _callable()
  , _class()
  , _type()
{
  if (orig._callable) {
    _callable = orig._callable->clone();
  }
  if (orig._class) {
    _class = orig._class->clone();
  }
}

Object&
Object::operator=(Object const& orig)
{
  if (this != &orig) {
    str = orig.str;
    num = orig.num;
    _boolean = orig._boolean;
    _callable = {};
    if (orig._callable) {
      _callable = orig._callable->clone();
    }
    _class = {};
    if (orig._class) {
      _class = orig._class->clone();
    }
  }

  return *this;
}

Object::~Object() = default;

Object
Object::null()
{
  return Object{};
}

} // namespace Lox