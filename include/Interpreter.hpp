#pragma once

#include "Expression.hpp"
#include "LoxRuntimeError.hpp"

namespace Lox {

class Interpreter : public ExpressionVisitor
{
public:
  std::string interpret(Expression const& expr)
  {
    try {
      return stringify(evaluate(expr));
    } catch (LoxRuntimeError err) {
      std::cout << err.what() << std::endl;
      return "";
    }
  }

  virtual std::any visitBinaryExpression(
    BinaryExpression const& expr) const override
  {
    auto lhs = evaluate(expr.lhs());
    auto rhs = evaluate(expr.rhs());

    switch (expr.op().type()) {
      case TokenType::BANG_EQUAL:
        return Object{ !isEqual(lhs, rhs) };
      case TokenType::EQUAL_EQUAL:
        return Object{ isEqual(lhs, rhs) };
      case TokenType::GREATER:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() > rhs.number() };
      case TokenType::GREATER_EQUAL:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() >= rhs.number() };
      case TokenType::LESS:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() < rhs.number() };
      case TokenType::LESS_EQUAL:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() <= rhs.number() };
      case TokenType::MINUS:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() - rhs.number() };
      case TokenType::PLUS:
        if (lhs.isNumber() && rhs.isNumber()) {
          return Object{ lhs.number() + rhs.number() };
        } else if (lhs.isString() && rhs.isString()) {
          return Object{ lhs.string() + rhs.string() };
        } else {
          throw LoxRuntimeError{
            expr.op(), "Operands must be two numbers or two strings"
          };
        }
      case TokenType::SLASH:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() / rhs.number() };
      case TokenType::STAR:
        checkNumberOperands(expr.op(), lhs, rhs);
        return Object{ lhs.number() * rhs.number() };
      default:
        return Object::null();
    }
  }

  virtual std::any visitGroupingExpression(
    GroupingExpression const& expr) const override
  {
    return evaluate(expr.expr());
  }

  virtual std::any visitLiteralExpression(
    LiteralExpression const& expr) const override
  {
    return expr.value();
  }

  virtual std::any visitUnaryExpression(
    UnaryExpression const& expr) const override
  {
    auto rhs = evaluate(expr.rhs());

    switch (expr.op().type()) {
      case TokenType::MINUS:
        checkNumberOperand(expr.op(), rhs);
        return Object{ -rhs.number() };
      case TokenType::BANG:
        return Object{ !isTruthy(rhs) };
      default:
        return Object::null();
    }
  }

private:
  Object evaluate(Expression const& expr) const
  {
    return std::any_cast<Object>(expr.accept(*this));
  }
  static bool isTruthy(Object const& obj)
  {
    if (obj.isNull())
      return false;
    else {
      if (obj.isBoolean()) {
        return obj.boolean();
      } else
        return true;
    }
  }
  static std::string stringify(Object const& obj)
  {
    if (obj.isNull())
      return "nil";
    else if (obj.isNumber()) {
      return std::to_string(obj.number());
    } else if (obj.isBoolean()) {
      return obj.boolean() == true ? "true" : "false";
    } else {
      // string assumed
      return obj.string();
    }
  }
  static bool isEqual(Object const& lhs, Object const& rhs)
  {
    if (lhs.type() != rhs.type()) {
      return false;
    } else {
      /**
       * lhs.type() == rhs.type()
       * so it's enough to check one side
       * */
      if (lhs.isNull())
        return true;
      else if (lhs.isBoolean()) {
        return lhs.boolean() == rhs.boolean();
      } else if (lhs.isNumber()) {
        return lhs.number() == rhs.number();
      } else {
        // string assumed
        return lhs.string() == rhs.string();
      }
    }
  }
  static void checkNumberOperand(Token const& op, Object const& operand)
  {
    if (!operand.isNumber()) {
      throw new LoxRuntimeError{ op, "Operand must be a number" };
    }
  }
  static void checkNumberOperands(Token const& op,
                                  Object const& lhs,
                                  Object const& rhs)
  {
    if (!lhs.isNumber() || !rhs.isNumber()) {
      throw new LoxRuntimeError{ op, "Operand must be a number" };
    }
  }
};

} // namespace Lox