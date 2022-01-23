#pragma once

#include <any>
#include <memory>

#include "Token.hpp"

namespace Lox {

class ExpressionVisitor;

class Expression
{
public:
  Expression() = default;
  Expression(Expression const&) = delete;
  Expression& operator=(Expression const&) = delete;
  virtual ~Expression() = default;

  template<typename T>
  T accept(ExpressionVisitor const& visitor) const
  {
    return std::any_cast<T>(accept(visitor));
  }

  virtual std::any accept(ExpressionVisitor const&) const = 0;
};

using Expr = std::unique_ptr<Expression>;

class BinaryExpression;
class GroupingExpression;
class LiteralExpression;
class UnaryExpression;

class ExpressionVisitor
{
public:
  virtual std::any visitBinaryExpression(BinaryExpression const&) const = 0;
  virtual std::any visitGroupingExpression(GroupingExpression const&) const = 0;
  virtual std::any visitLiteralExpression(LiteralExpression const&) const = 0;
  virtual std::any visitUnaryExpression(UnaryExpression const&) const = 0;

  ExpressionVisitor() = default;
  ExpressionVisitor(ExpressionVisitor const&) = delete;
  ExpressionVisitor& operator=(ExpressionVisitor const&) = delete;
  virtual ~ExpressionVisitor() = default;
};

class BinaryExpression : public Expression
{
public:
  Expression const& lhs() const { return *_lhs; }
  Token const& op() const { return _op; }
  Expression const& rhs() const { return *_rhs; }

  virtual std::any accept(ExpressionVisitor const& visitor) const override
  {
    return visitor.visitBinaryExpression(*this);
  }

  BinaryExpression(Expr&& in_lhs, Token in_op, Expr&& in_rhs)
    : _lhs(std::move(in_lhs))
    , _op(std::move(in_op))
    , _rhs(std::move(in_rhs))
  {}

private:
  Expr _lhs;
  Token _op;
  Expr _rhs;
};

class GroupingExpression : public Expression
{
public:
  Expression const& expr() const { return *_expr; }

  virtual std::any accept(ExpressionVisitor const& visitor) const override
  {
    return visitor.visitGroupingExpression(*this);
  }

  GroupingExpression(Expr&& in_expr)
    : _expr(std::move(in_expr))
  {}

private:
  Expr _expr;
};

class LiteralExpression : public Expression
{
public:
  Object const& value() const { return literal; }

  virtual std::any accept(ExpressionVisitor const& visitor) const override
  {
    return visitor.visitLiteralExpression(*this);
  }

  LiteralExpression(Object literal)
    : literal(literal)
  {}

private:
  Object literal;
};

class UnaryExpression : public Expression
{
public:
  Token const& op() const { return _op; }
  Expression const& rhs() const { return *_rhs; }

  virtual std::any accept(ExpressionVisitor const& visitor) const override
  {
    return visitor.visitUnaryExpression(*this);
  }

  UnaryExpression(Token in_op, Expr&& in_rhs)
    : _op(in_op)
    , _rhs(std::move(in_rhs))
  {}

private:
  Token _op;
  Expr _rhs;
};

} // namespace Lox