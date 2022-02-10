#pragma once

#include "Expression.hpp"

namespace Lox {

class StatementVisitor;

class Statement;
using Stmt = std::unique_ptr<Statement>;

class Statement
{
public:
  Statement() = default;
  Statement(Statement const&) = delete;
  Statement& operator=(Statement const&) = delete;
  virtual ~Statement() = default;

  virtual void accept(StatementVisitor&) const = 0;
  virtual Stmt clone() const = 0;
};

class BlockStatement;
class ClassDeclarationStatement;
class ExpressionStatement;
class PrintStatement;
class VarDeclarationStatement;
class IfStatement;
class WhileStatement;
class FunctionDeclarationStatement;
class ReturnStatement;

class StatementVisitor
{
public:
  virtual void visitBlockStatement(BlockStatement const&) = 0;
  virtual void visitClassDeclarationStatement(
    ClassDeclarationStatement const&) = 0;
  virtual void visitExpressionStatement(ExpressionStatement const&) = 0;
  virtual void visitPrintStatement(PrintStatement const&) = 0;
  virtual void visitVarDeclarationStatement(VarDeclarationStatement const&) = 0;
  virtual void visitIfStatement(IfStatement const&) = 0;
  virtual void visitWhileStatement(WhileStatement const&) = 0;
  virtual void visitFunctionDeclarationStatement(
    FunctionDeclarationStatement const&) = 0;
  virtual void visitReturnStatement(ReturnStatement const&) = 0;

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

  virtual Stmt clone() const override
  {
    return std::make_unique<PrintStatement>(expr->clone());
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

  virtual Stmt clone() const override
  {
    auto cloned_statements = std::vector<Stmt>{};
    for (auto& stmt : _statements) {
      cloned_statements.push_back(stmt->clone());
    }

    return std::make_unique<BlockStatement>(std::move(cloned_statements));
  }

  BlockStatement(std::vector<Stmt> in_statements)
    : _statements(std::move(in_statements))
  {}

private:
  std::vector<Stmt> _statements;
};

class ClassDeclarationStatement : public Statement
{
public:
  Token const& name() const { return _name; }
  std::vector<Stmt> const& methods() const { return _methods; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitClassDeclarationStatement(*this);
  }

  virtual Stmt clone() const override
  {
    auto cloned_methods = std::vector<Stmt>{};
    for (auto& method : _methods) {
      cloned_methods.push_back(method->clone());
    }

    return std::make_unique<ClassDeclarationStatement>(
      _name, std::move(cloned_methods));
  }

  ClassDeclarationStatement(Token in_name, std::vector<Stmt> in_methods)
    : _name(in_name)
    , _methods(std::move(in_methods))
  {}

private:
  Token _name;
  std::vector<Stmt> _methods;
};

class ExpressionStatement : public Statement
{
public:
  Expression const& expression() const { return *expr; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitExpressionStatement(*this);
  }

  virtual Stmt clone() const override
  {
    return std::make_unique<ExpressionStatement>(expr->clone());
  }

  ExpressionStatement(Expr&& expr)
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

  virtual Stmt clone() const override
  {
    return std::make_unique<IfStatement>(_condition->clone(),
                                         then_branch->clone(),
                                         else_branch ? else_branch->clone()
                                                     : nullptr);
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

  virtual Stmt clone() const override
  {
    return std::make_unique<WhileStatement>(_condition->clone(),
                                            _body->clone());
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

  virtual Stmt clone() const override
  {
    return std::make_unique<VarDeclarationStatement>(_name,
                                                     _initializer->clone());
  }

  VarDeclarationStatement(Token in_name, Expr in_initializer)
    : _name(in_name)
    , _initializer(std::move(in_initializer))
  {}

private:
  Token _name;
  Expr _initializer;
};

class FunctionDeclarationStatement : public Statement
{
public:
  Token const& name() const { return _name; }
  std::vector<Token> const& params() const { return _params; }
  std::vector<Stmt> const& body() const { return _body; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitFunctionDeclarationStatement(*this);
  }

  virtual Stmt clone() const override
  {
    auto cloned_body = std::vector<Stmt>{};
    for (auto& stmt : _body) {
      cloned_body.push_back(stmt->clone());
    }

    return std::make_unique<FunctionDeclarationStatement>(
      _name, _params, std::move(cloned_body));
  }

  FunctionDeclarationStatement(Token in_name,
                               std::vector<Token> in_params,
                               std::vector<Stmt> in_body)
    : _name(std::move(in_name))
    , _params(in_params)
    , _body(std::move(in_body))
  {}

private:
  Token _name;
  std::vector<Token> _params;
  std::vector<Stmt> _body;
};

class ReturnStatement : public Statement
{
public:
  Expression const& value() const { return *val; }

  virtual void accept(StatementVisitor& visitor) const override
  {
    visitor.visitReturnStatement(*this);
  }

  virtual Stmt clone() const override
  {
    return std::make_unique<ReturnStatement>(val->clone());
  }

  ReturnStatement(Expr&& val)
    : val(std::move(val))
  {}

private:
  Expr val;
};

} // namespace Lox