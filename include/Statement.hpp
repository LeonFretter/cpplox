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

class BlockStatement;
class ExpressionStatement;
class PrintStatement;
class VarDeclarationStatement;
class IfStatement;
class WhileStatement;

class StatementVisitor
{
public:
  virtual void visitBlockStatement(BlockStatement const&) = 0;
  virtual void visitExpressionStatement(ExpressionStatement const&) = 0;
  virtual void visitPrintStatement(PrintStatement const&) = 0;
  virtual void visitVarDeclarationStatement(VarDeclarationStatement const&) = 0;
  virtual void visitIfStatement(IfStatement const&) = 0;
  virtual void visitWhileStatement(WhileStatement const&) = 0;

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

class BlockStatement : public Statement
{
public:
  std::vector<Stmt> const& statements() const { return _statements; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitBlockStatement(*this);
  }

  BlockStatement(std::vector<Stmt> in_statements)
    : _statements(std::move(in_statements))
  {}

private:
  std::vector<Stmt> _statements;
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

class IfStatement : public Statement
{
public:
  Expression const& condition() const { return *_condition; }
  Statement const& thenBranch() const { return *then_branch; }
  bool hasElseBranch() const { return else_branch.operator bool(); }
  Statement const& elseBranch() const { return *else_branch; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitIfStatement(*this);
  }

  IfStatement(Expr in_condition, Stmt in_then_branch, Stmt in_else_branch)
    : _condition(std::move(in_condition))
    , then_branch(std::move(in_then_branch))
    , else_branch(std::move(in_else_branch))
  {}

private:
  Expr _condition;
  Stmt then_branch;
  Stmt else_branch;
};

class WhileStatement : public Statement
{
public:
  Expression const& condition() const { return *_condition; }
  Statement const& body() const { return *_body; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitWhileStatement(*this);
  }

  WhileStatement(Expr in_condition, Stmt in_body)
    : _condition(std::move(in_condition))
    , _body(std::move(in_body))
  {}

private:
  Expr _condition;
  Stmt _body;
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