#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include <ExpressionPrinter.hpp>
#include <Interpreter.hpp>
#include <Parser.hpp>
#include <Scanner.hpp>

std::vector<char>
readFromFile(char const* path_str)
{
  auto path = std::filesystem::path{ path_str };
  auto f = std::ifstream{ path, std::ios::ate };
  if (!f.is_open()) {
    throw std::runtime_error("File could not be opened");
  }

  auto sz = static_cast<size_t>(f.tellg());
  auto buffer = std::vector<char>(sz);
  f.seekg(0);
  f.read(buffer.data(), sz);

  return buffer;
}

template<typename T>
void
run(T const& src)
{
  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();
  auto parser = Lox::Parser{ tokens };
  auto expr = parser.parse();

  Lox::ExpressionPrinter{}.print(*expr);
  std::cout << Lox::Interpreter{}.interpret(*expr) << std::endl;
}

int
main(int argc, char** argv)
{
  if (argc > 2) {
    std::cout << "Usage: cpplox [file]" << std::endl;
    return EXIT_FAILURE;
  } else if (argc == 2) {
    auto buf = readFromFile(argv[1]);
    run(buf);
  } else {
    auto line = std::string{};
    while (std::getline(std::cin, line)) {
      run(line);
    }
  }
}