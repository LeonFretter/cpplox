#pragma once

#include <any>
#include <memory>
#include <vector>

#include "Token.hpp"

namespace Lox {

class ExpressionVisitor;
class Expression;

using Expr = std::unique_ptr<Expression>;

class Expression
{
public:
  Expression() = default;
  Expression(Expression const&) = delete;
  Expression& operator=(Expression const&) = delete;
  virtual ~Expression() = default;

  virtual std::any accept(ExpressionVisitor&) const = 0;
  virtual Expr clone() const = 0;
};

class AssignmentExpression;
class BinaryExpression;
class CallExpression;
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
  virtual std::any visitCallExpression(CallExpression const&) = 0;

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

  virtual Expr clone() const override
  {
    return std::make_unique<AssignmentExpression>(_name, _value->clone());
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

  virtual Expr clone() const override
  {
    return std::make_unique<BinaryExpression>(
      _lhs->clone(), _op, _rhs->clone());
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

class CallExpression : public Expression
{
public:
  std::vector<Expr> const& arguments() const { return args; }
  Expression const& callee() const { return *_callee; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
  {
    return visitor.visitCallExpression(*this);
  }

  virtual Expr clone() const override
  {
    auto cloned_args = std::vector<Expr>{};
    for (auto& arg : args) {
      cloned_args.push_back(arg->clone());
    }

    return std::make_unique<CallExpression>(_callee->clone(),
                                            std::move(cloned_args));
  }

  CallExpression(Expr in_callee, std::vector<Expr> args)
    : _callee(std::move(in_callee))
    , args(std::move(args))
  {}

private:
  Expr _callee;
  std::vector<Expr> args;
};

class GroupingExpression : public Expression
{
public:
  Expression const& expr() const { return *_expr; }

  virtual std::any accept(ExpressionVisitor& visitor) const override
  {
    return visitor.visitGroupingExpression(*this);
  }

  virtual Expr clone() const override
  {
    return std::make_unique<GroupingExpression>(_expr->clone());
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

  virtual Expr clone() const override
  {
    return std::make_unique<LiteralExpression>(literal);
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

  virtual Expr clone() const override
  {
    return std::make_unique<VariableExpression>(_name);
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

  virtual Expr clone() const override
  {
    return std::make_unique<UnaryExpression>(_op, _rhs->clone());
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