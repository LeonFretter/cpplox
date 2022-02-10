#pragma once

#include <memory>
#include <string>

namespace Lox {

class LoxClass
{
public:
  std::string const& toString() const noexcept { return name; }
  std::unique_ptr<LoxClass> clone() { return std::make_unique<LoxClass>(name); }

  LoxClass(std::string name)
    : name(name)
  {}

private:
  std::string name;
};

} // namespace Lox