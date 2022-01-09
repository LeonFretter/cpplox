#include <gtest/gtest.h>

#include <Token.hpp>

TEST(ObjectTest, EmptyObject)
{
  auto obj = Lox::Object{};

  EXPECT_FALSE(obj.isNumber());
  EXPECT_FALSE(obj.isString());
  EXPECT_FALSE(obj.operator bool());
}

TEST(ObjectTest, StringObject)
{
  auto str = std::string{ "hello" };

  auto obj = Lox::Object{ str };

  EXPECT_FALSE(obj.isNumber());
  EXPECT_TRUE(obj.isString());
  EXPECT_TRUE(obj.operator bool());

  EXPECT_EQ(obj.string(), str);
}

TEST(ObjectTest, NumberObject)
{
  auto num = 42.0;

  auto obj = Lox::Object{ num };

  EXPECT_TRUE(obj.isNumber());
  EXPECT_FALSE(obj.isString());
  EXPECT_TRUE(obj.operator bool());

  EXPECT_EQ(obj.number(), num);
}