#pragma once

#include "Expression.hpp"

namespace Lox {

class StatementVisitor;

class Statement
{
public:
  Statement() = default;
  Statement(Statement const&) = delete;
  Statement& operator=(Statement const&) = delete;
  virtual ~Statement() = default;

  virtual void accept(StatementVisitor&) const = 0;
};

using Stmt = std::unique_ptr<Statement>;

class PrintStatement;
class ExpressionStatement;
class VarDeclarationStatement;

class StatementVisitor
{
public:
  virtual void visitPrintStatement(PrintStatement const&) = 0;
  virtual void visitExpressionStatement(ExpressionStatement const&) = 0;
  virtual void visitVarDeclarationStatement(VarDeclarationStatement const&) = 0;

  StatementVisitor() = default;
  StatementVisitor(StatementVisitor const&) = delete;
  StatementVisitor& operator=(StatementVisitor const&) = delete;
  virtual ~StatementVisitor() = default;
};

class PrintStatement : public Statement
{
public:
  Expression const& expression() const { return *expr; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitPrintStatement(*this);
  }

  PrintStatement(Expr expr)
    : expr(std::move(expr))
  {}

private:
  Expr expr;
};

class ExpressionStatement : public Statement
{
public:
  Expression const& expression() const { return *expr; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitExpressionStatement(*this);
  }

  ExpressionStatement(Expr expr)
    : expr(std::move(expr))
  {}

private:
  Expr expr;
};

class VarDeclarationStatement : public Statement
{
public:
  Token const& name() const { return _name; }
  Expression const& initializer() const { return *_initializer; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitVarDeclarationStatement(*this);
  }

  VarDeclarationStatement(Token in_name, Expr in_initializer)
    : _name(in_name)
    , _initializer(std::move(in_initializer))
  {}

private:
  Token _name;
  Expr _initializer;
};

} // namespace Lox