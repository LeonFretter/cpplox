#pragma once

#include <map>

#include "Expression.hpp"
#include "Interpreter.hpp"
#include "Statement.hpp"

namespace Lox {

class Resolver
  : public ExpressionVisitor
  , StatementVisitor
{
  using ScopeMap = std::map<std::string, bool>;

public:
  void resolve(std::vector<Stmt> const&);

  virtual std::any visitAssignmentExpression(
    AssignmentExpression const&) override;
  virtual std::any visitBinaryExpression(BinaryExpression const&) override;
  virtual std::any visitCallExpression(CallExpression const&) override;
  virtual std::any visitGroupingExpression(GroupingExpression const&) override;
  virtual std::any visitLiteralExpression(LiteralExpression const&) override;
  virtual std::any visitVariableExpression(VariableExpression const&) override;
  virtual std::any visitUnaryExpression(UnaryExpression const&) override;

  virtual void visitBlockStatement(BlockStatement const&) override;
  virtual void visitClassDeclarationStatement(
    ClassDeclarationStatement const&) override;
  virtual void visitExpressionStatement(ExpressionStatement const&) override;
  virtual void visitPrintStatement(PrintStatement const&) override;
  virtual void visitVarDeclarationStatement(
    VarDeclarationStatement const&) override;
  virtual void visitIfStatement(IfStatement const&) override;
  virtual void visitWhileStatement(WhileStatement const&) override;
  virtual void visitFunctionDeclarationStatement(
    FunctionDeclarationStatement const&) override;
  virtual void visitReturnStatement(ReturnStatement const&) override;

  Resolver(Interpreter&);

private:
  void resolveFunction(FunctionDeclarationStatement const&);
  void beginScope();
  void endScope();

  void resolve(Statement const&);
  void resolve(Expression const&);
  void resolveLocal(Expression const&, Token const& name);

  void declare(Token const&);
  void define(Token const&);

private:
  Interpreter& interpreter;
  std::vector<ScopeMap> scopes;
};

} // namespace Lox