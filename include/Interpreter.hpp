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
  void interpret(std::vector<Stmt> const& statements);

  virtual void visitPrintStatement(PrintStatement const& stmt) override;

  virtual void visitExpressionStatement(
    ExpressionStatement const& stmt) override;

  virtual void visitVarDeclarationStatement(
    VarDeclarationStatement const& stmt) override;

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

  Interpreter() = default;

private:
  void execute(Statement const& stmt);

  Object evaluate(Expression const& expr);

  static bool isTruthy(Object const& obj);

  static std::string stringify(Object const& obj);

  static bool isEqual(Object const& lhs, Object const& rhs);

  static void checkNumberOperand(Token const& op, Object const& operand);

  static void checkNumberOperands(Token const& op,
                                  Object const& lhs,
                                  Object const& rhs);

private:
  Environment env;
};

} // namespace Lox