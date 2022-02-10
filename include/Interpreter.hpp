#pragma once

#include <vector>

#include "Environment.hpp"
#include "LoxRuntimeError.hpp"
#include "Statement.hpp"

namespace Lox {

class Interpreter
  : public ExpressionVisitor
  , public StatementVisitor
{
public:
  Environment& environment();

  void interpret(std::vector<Stmt> const& statements);

  void resolve(Token const& name, size_t depth);

  virtual void visitBlockStatement(BlockStatement const& stmt) override;
  virtual void visitClassDeclarationStatement(
    ClassDeclarationStatement const&) override;

  virtual void visitExpressionStatement(
    ExpressionStatement const& stmt) override;

  virtual void visitPrintStatement(PrintStatement const& stmt) override;

  virtual void visitVarDeclarationStatement(
    VarDeclarationStatement const& stmt) override;

  virtual void visitIfStatement(IfStatement const&) override;

  virtual void visitWhileStatement(WhileStatement const&) override;

  virtual void visitFunctionDeclarationStatement(
    FunctionDeclarationStatement const&) override;

  virtual void visitReturnStatement(ReturnStatement const&) override;

  virtual std::any visitAssignmentExpression(
    AssignmentExpression const& expr) override;

  virtual std::any visitBinaryExpression(BinaryExpression const& expr) override;

  virtual std::any visitGroupingExpression(
    GroupingExpression const& expr) override;

  virtual std::any visitLiteralExpression(
    LiteralExpression const& expr) override;

  virtual std::any visitVariableExpression(
    VariableExpression const& expr) override;

  virtual std::any visitUnaryExpression(UnaryExpression const& expr) override;

  virtual std::any visitCallExpression(CallExpression const&) override;

  void executeBlock(std::vector<Stmt> const&, SharedEnv);

  Interpreter();

private:
  void execute(Statement const& stmt);

  Object evaluate(Expression const& expr);

  Object lookupVariable(Token const& name, Expression const&);

  static bool isTruthy(Object const& obj);

  static std::string stringify(Object const& obj);

  static bool isEqual(Object const& lhs, Object const& rhs);

  static void checkNumberOperand(Token const& op, Object const& operand);

  static void checkNumberOperands(Token const& op,
                                  Object const& lhs,
                                  Object const& rhs);

private:
  SharedEnv globals;
  SharedEnv env;
  std::map<std::string, size_t> locals;
};

} // namespace Lox