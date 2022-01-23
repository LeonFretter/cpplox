#include <Interpreter.hpp>

namespace Lox {

void
Interpreter::interpret(std::vector<Stmt> const& statements)
{
  try {
    for (auto& stmt : statements) {
      execute(*stmt);
    }
  } catch (LoxRuntimeError err) {
    std::cout << err.what() << std::endl;
  }
}

void
Interpreter::visitPrintStatement(PrintStatement const& stmt)
{
  auto val = evaluate(stmt.expression());
  std::cout << stringify(val) << std::endl;
}

void
Interpreter::visitExpressionStatement(ExpressionStatement const& stmt)
{
  evaluate(stmt.expression());
}

void
Interpreter::visitVarDeclarationStatement(VarDeclarationStatement const& stmt)
{
  env.define(stmt.name().lexeme(), evaluate(stmt.initializer()));
}

std::any
Interpreter::visitAssignmentExpression(AssignmentExpression const& expr)
{
  auto val = evaluate(expr.value());
  env.assign(expr.name(), val);
  return val;
}

std::any
Interpreter::visitBinaryExpression(BinaryExpression const& expr)
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
        throw LoxRuntimeError{ expr.op(),
                               "Operands must be two numbers or two strings" };
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

std::any
Interpreter::visitGroupingExpression(GroupingExpression const& expr)
{
  return evaluate(expr.expr());
}

std::any
Interpreter::visitLiteralExpression(LiteralExpression const& expr)
{
  return expr.value();
}

std::any
Interpreter::visitVariableExpression(VariableExpression const& expr)
{
  return env.get(expr.name());
}

std::any
Interpreter::visitUnaryExpression(UnaryExpression const& expr)
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

void
Interpreter::execute(Statement const& stmt)
{
  stmt.accept(*this);
}

Object
Interpreter::evaluate(Expression const& expr)
{
  return std::any_cast<Object>(expr.accept(*this));
}

bool
Interpreter::isTruthy(Object const& obj)
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

std::string
Interpreter::stringify(Object const& obj)
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

bool
Interpreter::isEqual(Object const& lhs, Object const& rhs)
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

void
Interpreter::checkNumberOperand(Token const& op, Object const& operand)
{
  if (!operand.isNumber()) {
    throw LoxRuntimeError{ op, "Operand must be a number" };
  }
}

void
Interpreter::checkNumberOperands(Token const& op,
                                 Object const& lhs,
                                 Object const& rhs)
{
  if (!lhs.isNumber() || !rhs.isNumber()) {
    throw LoxRuntimeError{ op, "Operand must be a number" };
  }
}

} // namespace Lox