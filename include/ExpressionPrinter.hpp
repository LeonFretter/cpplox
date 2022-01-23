#pragma once

#include <vector>

#include "Expression.hpp"

namespace Lox {

class ExpressionPrinter : public ExpressionVisitor
{
public:
  void print(Expression const& expr) const
  {
    std::cout << expr.accept<std::string>(*this) << std::endl;
  }

  virtual std::any visitBinaryExpression(
    BinaryExpression const& expr) const override
  {
    return parenthesize(expr.op().lexeme(), { &expr.lhs(), &expr.rhs() });
  }
  virtual std::any visitGroupingExpression(
    GroupingExpression const& expr) const override
  {
    return parenthesize("group", { &expr.expr() });
  }
  virtual std::any visitLiteralExpression(
    LiteralExpression const& expr) const override
  {
    auto const& obj = expr.value();
    if (!obj) {
      return "nil";
    } else if (obj.isBoolean()) {
      return obj.boolean() == true ? std::string{ "true" }
                                   : std::string{ "false" };
    } else if (obj.isNumber()) {
      return std::to_string(obj.number());
    } else {
      return obj.string();
    }
  }
  virtual std::any visitUnaryExpression(
    UnaryExpression const& expr) const override
  {
    return parenthesize(expr.op().lexeme(), { &expr.rhs() });
  }

private:
  std::string parenthesize(std::string name,
                           std::vector<Expression const*> expressions) const
  {
    auto res = std::string{};
    res += "(" + name;
    for (auto* expr : expressions) {
      res += " ";
      res += expr->accept<std::string>(*this);
    }
    res += ")";

    return res;
  }
};

} // namespace Lox