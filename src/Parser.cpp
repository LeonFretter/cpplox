#include <Parser.hpp>

namespace Lox {

std::vector<Stmt>
Parser::parse()
{
  auto statements = std::vector<Stmt>{};
  while (!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}

Parser::Parser(std::vector<Token> tokens)
  : tokens(tokens)
  , current(0UL)
{}

Stmt
Parser::declaration()
{
  if (match(TokenType::VAR))
    return varDeclaration();
  else
    return statement();
}

Stmt
Parser::statement()
{
  if (match(TokenType::PRINT))
    return printStatement();

  return expressionStatement();
}

Stmt
Parser::varDeclaration()
{
  auto name = consume(TokenType::IDENTIFIER, "Expect variable name");

  auto initializer = static_cast<std::unique_ptr<Expression>>(
    std::make_unique<LiteralExpression>(Object::null()));
  if (match(TokenType::EQUAL)) {
    initializer = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ; after variable declaration");
  return std::make_unique<VarDeclarationStatement>(name,
                                                   std::move(initializer));
}

Stmt
Parser::printStatement()
{
  auto val = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after statement.");
  return std::make_unique<PrintStatement>(std::move(val));
}

Stmt
Parser::expressionStatement()
{
  auto expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_unique<ExpressionStatement>(std::move(expr));
}

Expr
Parser::expression()
{
  return assignment();
}

Expr
Parser::assignment()
{
  auto expr = equality();

  if (match(TokenType::EQUAL)) {
    auto val = assignment();

    // TODO: catch failed cast
    auto name = dynamic_cast<VariableExpression const&>(*expr).name();

    return std::make_unique<AssignmentExpression>(name, std::move(val));
  }

  return expr;
}

Expr
Parser::equality()
{
  auto expr = comparison();

  while (matchOneOf({ TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL })) {
    auto op = previous();
    auto rhs = comparison();
    // previous expression becomes lhs
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
}

Expr
Parser::comparison()
{
  auto expr = term();

  while (matchOneOf({ TokenType::GREATER,
                      TokenType::GREATER_EQUAL,
                      TokenType::LESS,
                      TokenType::LESS_EQUAL })) {
    auto op = previous();
    auto rhs = term();
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
}

Expr
Parser::term()
{
  auto expr = factor();

  while (matchOneOf({ TokenType::MINUS, TokenType::PLUS })) {
    auto op = previous();
    auto rhs = factor();
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
}

Expr
Parser::factor()
{
  auto expr = unary();

  while (matchOneOf({ TokenType::SLASH, TokenType::STAR })) {
    auto op = previous();
    auto rhs = unary();
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
}

Expr
Parser::unary()
{
  if (matchOneOf({ TokenType::BANG, TokenType::MINUS })) {
    auto op = previous();
    auto rhs = unary();
    return std::make_unique<UnaryExpression>(op, std::move(rhs));
  }

  return primary();
}

Expr
Parser::primary()
{
  if (match(TokenType::IDENTIFIER)) {
    return std::make_unique<VariableExpression>(previous());
  }
  if (match(TokenType::FALSE)) {
    return std::make_unique<LiteralExpression>(Object{ false });
  }
  if (match(TokenType::TRUE)) {
    return std::make_unique<LiteralExpression>(Object{ true });
  }
  if (match(TokenType::NIL)) {
    return std::make_unique<LiteralExpression>(Object{});
  }
  if (matchOneOf({ TokenType::NUMBER, TokenType::STRING })) {
    return std::make_unique<LiteralExpression>(previous().literal());
  }

  if (match(TokenType::LEFT_PAREN)) {
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Unterminated parentheses");
    return std::make_unique<GroupingExpression>(std::move(expr));
  }

  throw std::runtime_error("Expected an expression.");
}

bool
Parser::match(TokenType token_type)
{
  return matchOneOf({ token_type });
}
bool
Parser::matchOneOf(std::vector<TokenType> token_types)
{
  for (auto t : token_types) {
    if (check(t)) {
      advance();
      return true;
    }
  }
  return false;
}

bool
Parser::check(TokenType type) const
{
  return peek().type() == type;
}

Token
Parser::consume(TokenType token_type, std::string message)
{
  if (check(token_type))
    return advance();
  else
    throw std::runtime_error(message);
}

Token
Parser::advance()
{
  if (!isAtEnd()) {
    return tokens.at(current++);
  }
  return Token{ TokenType::END_OF_FILE, "" };
}

bool
Parser::isAtEnd() const
{
  return peek().type() == TokenType::END_OF_FILE;
}

Token
Parser::peek() const
{
  return tokens.at(current);
}

Token
Parser::previous() const
{
  return tokens.at(current - 1UL);
}

} // namespace Lox