#pragma once

#include <vector>

#include "Expression.hpp"

namespace Lox {

class ExpressionPrinter : public ExpressionVisitor
{
public:
  void print(Expression const& expr)
  {

    std::cout << std::any_cast<std::string>(expr.accept(*this)) << std::endl;
  }

  virtual std::any visitBinaryExpression(BinaryExpression const& expr) override
  {
    return parenthesize(expr.op().lexeme(), { &expr.lhs(), &expr.rhs() });
  }
  virtual std::any visitGroupingExpression(
    GroupingExpression const& expr) override
  {
    return parenthesize("group", { &expr.expr() });
  }
  virtual std::any visitLiteralExpression(
    LiteralExpression const& expr) override
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
  // TODO: variableExpression

  virtual std::any visitUnaryExpression(UnaryExpression const& expr) override
  {
    return parenthesize(expr.op().lexeme(), { &expr.rhs() });
  }

private:
  std::string parenthesize(std::string name,
                           std::vector<Expression const*> expressions)
  {
    auto res = std::string{};
    res += "(" + name;
    for (auto* expr : expressions) {
      res += " ";
      res += std::any_cast<std::string>(expr->accept(*this));
    }
    res += ")";

    return res;
  }
};

} // namespace Lox