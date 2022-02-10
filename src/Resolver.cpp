#include <Resolver.hpp>

namespace Lox {

std::any
Resolver::visitAssignmentExpression(AssignmentExpression const& expr)
{
  resolve(expr.value());
  resolveLocal(expr, expr.name());
  return Object::null();
}

std::any
Resolver::visitBinaryExpression(BinaryExpression const& expr)
{
  resolve(expr.lhs());
  resolve(expr.rhs());
  return Object::null();
}

std::any
Resolver::visitCallExpression(CallExpression const& expr)
{
  resolve(expr.callee());
  for (auto& arg : expr.arguments()) {
    resolve(*arg);
  }
  return Object::null();
}

std::any
Resolver::visitGroupingExpression(GroupingExpression const& expr)
{
  resolve(expr.expr());
  return Object::null();
}

std::any
Resolver::visitLiteralExpression(LiteralExpression const& expr)
{
  return Object::null();
}

std::any
Resolver::visitVariableExpression(VariableExpression const& expr)
{
  if (!scopes.empty() && scopes.back().contains(expr.name().lexeme()) &&
      scopes.back().at(expr.name().lexeme()) == false) {
    throw std::runtime_error(
      "Can't read local variable in its own initializer");
  }

  resolveLocal(expr, expr.name());
  return Object::null();
}

std::any
Resolver::visitUnaryExpression(UnaryExpression const& expr)
{
  resolve(expr.rhs());
  return Object::null();
}

void
Resolver::visitBlockStatement(BlockStatement const& stmt)
{
  beginScope();
  resolve(stmt.statements());
  endScope();
}

void
Resolver::visitClassDeclarationStatement(ClassDeclarationStatement const& stmt)
{
  declare(stmt.name());
  define(stmt.name());
}

void
Resolver::visitExpressionStatement(ExpressionStatement const& stmt)
{
  resolve(stmt.expression());
}

void
Resolver::visitPrintStatement(PrintStatement const& stmt)
{
  resolve(stmt.expression());
}

void
Resolver::visitIfStatement(IfStatement const& stmt)
{
  resolve(stmt.condition());
  resolve(stmt.thenBranch());
  if (stmt.hasElseBranch()) {
    resolve(stmt.elseBranch());
  }
}

void
Resolver::visitWhileStatement(WhileStatement const& stmt)
{
  resolve(stmt.condition());
  resolve(stmt.body());
}

void
Resolver::visitVarDeclarationStatement(VarDeclarationStatement const& stmt)
{
  declare(stmt.name());
  resolve(stmt.initializer());

  define(stmt.name());
}

void
Resolver::visitFunctionDeclarationStatement(
  FunctionDeclarationStatement const& stmt)
{
  /**
   * define immmediately after declaration,
   * to allow function to refer to itself inside its own body.
   * */
  declare(stmt.name());
  define(stmt.name());
  resolveFunction(stmt);
}

void
Resolver::visitReturnStatement(ReturnStatement const& stmt)
{
  resolve(stmt.value());
}

Resolver::Resolver(Interpreter& interpreter)
  : interpreter(interpreter)
  , scopes()
{}

void
Resolver::resolveFunction(FunctionDeclarationStatement const& fn)
{
  beginScope();
  for (auto& param : fn.params()) {
    declare(param);
    define(param);
  }
  resolve(fn.body());
  endScope();
}

void
Resolver::beginScope()
{
  scopes.push_back(ScopeMap{});
}

void
Resolver::endScope()
{
  scopes.pop_back();
}

void
Resolver::resolve(std::vector<Stmt> const& statements)
{
  for (auto& stmt : statements) {
    resolve(*stmt);
  }
}

void
Resolver::resolve(Statement const& stmt)
{
  stmt.accept(*this);
}

void
Resolver::resolve(Expression const& expr)
{
  expr.accept(*this);
}

void
Resolver::resolveLocal(Expression const& expr, Token const& name)
{
  for (auto i = scopes.size() - 1UL; i != SIZE_MAX; i--) {
    if (scopes.at(i).contains(name.lexeme())) {
      interpreter.resolve(name, scopes.size() - 1UL - i);
      return;
    }
  }
}

void
Resolver::declare(Token const& name)
{
  if (scopes.empty())
    return;

  auto& scope = scopes.back();
  if (scope.contains(name.lexeme())) {
    throw std::runtime_error("Redeclaration of variable" + name.lexeme());
  } else {
    scope.emplace(name.lexeme(), false);
  }
}

void
Resolver::define(Token const& name)
{
  if (scopes.empty())
    return;
  else
    scopes.back().at(name.lexeme()) = true;
}

} // namespace Lox