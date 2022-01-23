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

  virtual std::any accept(ExpressionVisitor&) const = 0;
};

using Expr = std::unique_ptr<Expression>;

class AssignmentExpression;
class BinaryExpression;
class GroupingExpression;
class LiteralExpression;
class UnaryExpression;
class VariableExpression;

class ExpressionVisitor
{
public:
  virtual std::any visitAssignmentExpression(AssignmentExpression const&) = 0;
  virtual std::any visitBinaryExpression(BinaryExpression const&) = 0;
  virtual std::any visitGroupingExpression(GroupingExpression const&) = 0;
  virtual std::any visitLiteralExpression(LiteralExpression const&) = 0;
  virtual std::any visitVariableExpression(VariableExpression const&) = 0;
  virtual std::any visitUnaryExpression(UnaryExpression const&) = 0;

  ExpressionVisitor() = default;
  ExpressionVisitor(ExpressionVisitor const&) = delete;
  ExpressionVisitor& operator=(ExpressionVisitor const&) = delete;
  virtual ~ExpressionVisitor() = default;
};

class AssignmentExpression : public Expression
{
public:
  Token name() const { return _name; }
  Expression const& value() const { return *_value; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
  {
    return visitor.visitAssignmentExpression(*this);
  }

  AssignmentExpression(Token in_name, Expr in_value)
    : _name(in_name)
    , _value(std::move(in_value))
  {}

private:
  Token _name;
  Expr _value;
};

class BinaryExpression : public Expression
{
public:
  Expression const& lhs() const { return *_lhs; }
  Token const& op() const { return _op; }
  Expression const& rhs() const { return *_rhs; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
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

  virtual std::any accept(ExpressionVisitor& visitor) const override
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

  virtual std::any accept(ExpressionVisitor& visitor) const override
  {
    return visitor.visitLiteralExpression(*this);
  }

  LiteralExpression(Object literal)
    : literal(literal)
  {}

private:
  Object literal;
};

class VariableExpression : public Expression
{
public:
  Token const& name() const { return _name; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
  {
    return visitor.visitVariableExpression(*this);
  }

  VariableExpression(Token in_name)
    : _name(in_name)
  {}

private:
  Token _name;
};

class UnaryExpression : public Expression
{
public:
  Token const& op() const { return _op; }
  Expression const& rhs() const { return *_rhs; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
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