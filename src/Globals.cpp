#include <chrono>

#include <Callable.hpp>
#include <Globals.hpp>
#include <Statement.hpp>

namespace Lox {

void
defineGlobals(Environment& env)
{
  using namespace std::chrono;

  env.define(
    "clock",
    Object{ std::make_unique<NativeFunction>(
      [](std::vector<Object> const&) {
        return Object{ static_cast<double>(
          duration_cast<milliseconds>(system_clock::now().time_since_epoch())
            .count()) };
      },
      0UL) });
}

} // namespace Lox