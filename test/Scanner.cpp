#include <Scanner.hpp>
#include <gtest/gtest.h>

using TT = Lox::TokenType;

TEST(ScannerTest, OneCharTokens)
{
  auto src =
    std::vector<char>{ '(', ')', '{', '}', ',', '.', '-', '+', ';', '*' };

  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();

  // tokens should contain EOF after all other tokens
  EXPECT_EQ(tokens.size(), src.size() + 1UL);

  for (auto i = 0UL; i < src.size(); ++i) {
    auto src_str = std::string{ src.at(i) };
    auto token_str = tokens.at(i).lexeme();
    EXPECT_EQ(src_str, token_str);
  }

  for (auto& token : tokens) {
    EXPECT_FALSE(token.literal().operator bool());
  }

  EXPECT_EQ(tokens.at(0).type(), TT::LEFT_PAREN);
  EXPECT_EQ(tokens.at(1).type(), TT::RIGHT_PAREN);
  EXPECT_EQ(tokens.at(2).type(), TT::LEFT_BRACE);
  EXPECT_EQ(tokens.at(3).type(), TT::RIGHT_BRACE);
  EXPECT_EQ(tokens.at(4).type(), TT::COMMA);
  EXPECT_EQ(tokens.at(5).type(), TT::DOT);
  EXPECT_EQ(tokens.at(6).type(), TT::MINUS);
  EXPECT_EQ(tokens.at(7).type(), TT::PLUS);
  EXPECT_EQ(tokens.at(8).type(), TT::SEMICOLON);
  EXPECT_EQ(tokens.at(9).type(), TT::STAR);

  EXPECT_EQ(tokens.at(10).type(), TT::END_OF_FILE);
}

TEST(ScannerTest, TwoCharTokens)
{
  auto src = std::string{ "!= == <= >=" };

  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), 5UL);
  EXPECT_EQ(tokens.at(4UL).type(), TT::END_OF_FILE);

  EXPECT_EQ(tokens.at(0UL).type(), TT::BANG_EQUAL);
  EXPECT_EQ(tokens.at(0UL).lexeme(), "!=");

  EXPECT_EQ(tokens.at(1UL).type(), TT::EQUAL_EQUAL);
  EXPECT_EQ(tokens.at(1UL).lexeme(), "==");

  EXPECT_EQ(tokens.at(2UL).type(), TT::LESS_EQUAL);
  EXPECT_EQ(tokens.at(2UL).lexeme(), "<=");

  EXPECT_EQ(tokens.at(3UL).type(), TT::GREATER_EQUAL);
  EXPECT_EQ(tokens.at(3UL).lexeme(), ">=");
}

TEST(ScannerTest, WithComments)
{
  auto src =
    std::string{ "; // This is a comment \n + // and another one \n -" };

  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), 4UL);
  EXPECT_EQ(tokens.at(3UL).type(), TT::END_OF_FILE);

  EXPECT_EQ(tokens.at(0UL).type(), TT::SEMICOLON);
  EXPECT_EQ(tokens.at(1UL).type(), TT::PLUS);
  EXPECT_EQ(tokens.at(2UL).type(), TT::MINUS);

  EXPECT_EQ(tokens.at(0UL).lexeme(), ";");
  EXPECT_EQ(tokens.at(1UL).lexeme(), "+");
  EXPECT_EQ(tokens.at(2UL).lexeme(), "-");
}

TEST(ScannerTest, Identifiers)
{
  auto identifiers =
    std::vector<std::string>{ "some", "random", "identifiers" };

  auto src_str = std::string{};

  std::for_each(identifiers.begin(),
                identifiers.end(),
                [&src_str](std::string const& word) { src_str += " " + word; });

  auto scanner = Lox::Scanner{ src_str };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), identifiers.size() + 1UL);
  EXPECT_EQ(tokens.at(tokens.size() - 1UL).type(), TT::END_OF_FILE);

  for (auto i = 0UL; i < identifiers.size(); ++i) {
    auto const& t = tokens.at(i);

    EXPECT_EQ(t.type(), TT::IDENTIFIER);
    EXPECT_EQ(t.lexeme(), identifiers.at(i));
  }
}

TEST(ScannerTest, ReservedKeywords)
{
  auto src = std::string{ "and else for if" };

  auto scanner = Lox::Scanner{ src };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), 5UL);

  EXPECT_EQ(tokens.at(0UL).type(), TT::AND);
  EXPECT_EQ(tokens.at(0UL).lexeme(), "and");

  EXPECT_EQ(tokens.at(1UL).type(), TT::ELSE);
  EXPECT_EQ(tokens.at(1UL).lexeme(), "else");

  EXPECT_EQ(tokens.at(2UL).type(), TT::FOR);
  EXPECT_EQ(tokens.at(2UL).lexeme(), "for");

  EXPECT_EQ(tokens.at(3UL).type(), TT::IF);
  EXPECT_EQ(tokens.at(3UL).lexeme(), "if");
}

TEST(ScannerTest, NumberLiterals)
{
  auto numbers = std::vector<std::string>{ "123", "42.0", "69.666" };

  auto src_str = std::string{};
  std::for_each(numbers.begin(),
                numbers.end(),
                [&src_str](std::string const& word) { src_str += " " + word; });

  auto scanner = Lox::Scanner{ src_str };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), 4UL);
  EXPECT_EQ(tokens.at(3UL).type(), TT::END_OF_FILE);

  for (auto i = 0UL; i < 3UL; ++i) {
    auto& t = tokens.at(i);
    EXPECT_EQ(t.type(), TT::NUMBER);

    auto& literal = t.literal();
    EXPECT_TRUE(literal.operator bool());
    EXPECT_TRUE(literal.isNumber());
    EXPECT_FALSE(literal.isString());
    EXPECT_EQ(numbers.at(i), t.lexeme());
    EXPECT_EQ(std::stod(numbers.at(i)), literal.number());
  }
}

TEST(ScannerTest, StringLiterals)
{
  auto src_str = std::string{ "\"hello\" \"world\"" };

  auto scanner = Lox::Scanner{ src_str };
  auto tokens = scanner.scanTokens();

  EXPECT_EQ(tokens.size(), 3UL);
  EXPECT_EQ(tokens.at(2UL).type(), TT::END_OF_FILE);

  auto const& t1 = tokens.at(0UL);
  EXPECT_EQ(t1.type(), TT::STRING);
  auto const& l1 = t1.literal();
  EXPECT_FALSE(l1.isNumber());
  EXPECT_TRUE(l1.isString());
  EXPECT_EQ(t1.lexeme(), "\"hello\"");
  EXPECT_EQ(l1.string(), "hello");

  auto const& t2 = tokens.at(1UL);
  EXPECT_EQ(t2.type(), TT::STRING);
  auto const& l2 = t2.literal();
  EXPECT_FALSE(l2.isNumber());
  EXPECT_TRUE(l2.isString());
  EXPECT_EQ(t2.lexeme(), "\"world\"");
  EXPECT_EQ(l2.string(), "world");
}