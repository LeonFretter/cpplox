#include <Globals.hpp>
#include <Interpreter.hpp>

#include <Callable.hpp>
#include <LoxClass.hpp>

namespace Lox {

Environment&
Interpreter::environment()
{
  return *env;
}

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
Interpreter::resolve(Token const& name, size_t depth)
{
  locals.emplace(name.lexeme(), depth);
}

void
Interpreter::visitBlockStatement(BlockStatement const& stmt)
{
  executeBlock(stmt.statements(), std::make_shared<Environment>(env));
}

void
Interpreter::visitExpressionStatement(ExpressionStatement const& stmt)
{
  evaluate(stmt.expression());
}

void
Interpreter::visitPrintStatement(PrintStatement const& stmt)
{
  std::cout << "env: " << env->toString() << std::endl;
  auto val = evaluate(stmt.expression());
  std::cout << stringify(val) << std::endl;
}

void
Interpreter::visitVarDeclarationStatement(VarDeclarationStatement const& stmt)
{
  env->define(stmt.name().lexeme(), evaluate(stmt.initializer()));
}

void
Interpreter::visitIfStatement(IfStatement const& stmt)
{
  if (isTruthy(evaluate(stmt.condition()))) {
    execute(stmt.thenBranch());
  } else if (stmt.hasElseBranch()) {
    execute(stmt.elseBranch());
  }
}

void
Interpreter::visitWhileStatement(WhileStatement const& stmt)
{
  while (isTruthy(evaluate(stmt.condition()))) {
    execute(stmt.body());
  }
}

void
Interpreter::visitFunctionDeclarationStatement(
  FunctionDeclarationStatement const& stmt)
{
  auto func = std::make_unique<LoxFunction>(
    std::unique_ptr<FunctionDeclarationStatement>{
      dynamic_cast<FunctionDeclarationStatement*>(stmt.clone().release()) },
    env);
  env->define(stmt.name().lexeme(), Object{ std::move(func) });
}

void
Interpreter::visitClassDeclarationStatement(
  ClassDeclarationStatement const& stmt)
{
  env->define(stmt.name().lexeme(), Object::null());
  auto klass = std::make_unique<LoxClass>(stmt.name().lexeme());
  env->assign(stmt.name(), Object{ std::move(klass) });
}

void
Interpreter::visitReturnStatement(ReturnStatement const& stmt)
{
  throw ReturnValue{ evaluate(stmt.value()) };
}

std::any
Interpreter::visitAssignmentExpression(AssignmentExpression const& expr)
{
  auto val = evaluate(expr.value());

  if (locals.contains(expr.name().lexeme())) {
    env->assignAt(locals.at(expr.name().lexeme()), expr.name(), val);
  } else {
    env->assign(expr.name(), val);
  }

  return val;
}

std::any
Interpreter::visitBinaryExpression(BinaryExpression const& expr)
{
  auto op_type = expr.op().type();

  if (op_type == TokenType::AND || op_type == TokenType::OR) {
    auto lhs = evaluate(expr.lhs());

    if (expr.op().type() == TokenType::OR) {
      if (isTruthy(lhs))
        return lhs;
    } else {
      if (!isTruthy(lhs)) {
        return lhs;
      }
    }

    return evaluate(expr.rhs());

  } else {

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
          throw LoxRuntimeError{
            expr.op(), "Operands must be two numbers or two strings"
          };
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
  return lookupVariable(expr.name(), expr);
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

std::any
Interpreter::visitCallExpression(CallExpression const& expr)
{
  auto callee = evaluate(expr.callee());
  auto args = std::vector<Object>{};
  for (auto& arg : expr.arguments()) {
    args.push_back(evaluate(*arg));
  }

  return callee.callable().call(*this, args);
}

Interpreter::Interpreter()
  : globals(std::make_shared<Environment>())
  , env(globals)
{
  defineGlobals(*globals);
}

void
Interpreter::execute(Statement const& stmt)
{
  stmt.accept(*this);
}

void
Interpreter::executeBlock(std::vector<Stmt> const& statements,
                          SharedEnv block_env)
{
  auto prev = env;

  try {
    std::cout << "enter block" << std::endl;
    env = block_env;

    for (auto& stmt : statements) {
      execute(*stmt);
    }
  } catch (std::exception& e) {
    std::cout << "exit block" << std::endl;
    env = prev;
    throw;
  }

  std::cout << "exit block" << std::endl;
  env = prev;
}

Object
Interpreter::evaluate(Expression const& expr)
{
  return std::any_cast<Object>(expr.accept(*this));
}

Object
Interpreter::lookupVariable(Token const& name, Expression const& expr)
{
  // how does this work after having exited scope??!
  if (locals.contains(name.lexeme())) {
    return env->getAt(locals.at(name.lexeme()), name);
  } else {
    return globals->get(name);
  }
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
  return obj.toString();
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