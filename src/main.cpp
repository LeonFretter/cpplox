#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

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

void
run(std::vector<char> const& src)
{
  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();

  for (auto& token : tokens) {
    std::cout << token << std::endl;
  }
}

int
main(int argc, char** argv)
{
  if (argc > 1) {
    std::cout << "Usage: cpplox [file]" << std::endl;
    return EXIT_FAILURE;
  } else if (argc == 1) {
    auto buf = readFromFile(argv[0]);
    run(buf);
  } else {
    // TODO: run prompt
  }
}