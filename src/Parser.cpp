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

std::vector<Stmt>
Parser::block()
{
  auto statements = std::vector<Stmt>{};

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

Stmt
Parser::classDeclaration()
{
  auto name = consume(TokenType::IDENTIFIER, "Expect class name");
  consume(TokenType::LEFT_BRACE, "Expect '{' before class body");

  auto methods = std::vector<Stmt>{};

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    methods.push_back(function());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after class body");

  return std::make_unique<ClassDeclarationStatement>(name, std::move(methods));
}

Stmt
Parser::declaration()
{
  if (match(TokenType::CLASS))
    return classDeclaration();
  else if (match(TokenType::FUN))
    return function();
  else if (match(TokenType::VAR))
    return varDeclaration();
  else
    return statement();
}

Stmt
Parser::function()
{
  auto name = consume(TokenType::IDENTIFIER, "Expect function name");
  consume(TokenType::LEFT_PAREN, "Expect '(' after function name");
  auto params = std::vector<Token>{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name"));
    } while (match(TokenType::COMMA));
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after params");

  consume(TokenType::LEFT_BRACE, "Expect '{' before function body");
  auto body = block();
  return std::make_unique<FunctionDeclarationStatement>(
    name, params, std::move(body));
}

Stmt
Parser::returnStatement()
{
  // we consume the 'return'
  previous();

  auto val =
    static_cast<Expr>(std::make_unique<LiteralExpression>(Object::null()));
  if (!check(TokenType::SEMICOLON)) {
    val = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after return statement.");
  return std::make_unique<ReturnStatement>(std::move(val));
}

Stmt
Parser::statement()
{
  if (match(TokenType::FOR)) {
    return forStatement();
  } else if (match(TokenType::IF)) {
    return ifStatement();
  } else if (match(TokenType::PRINT))
    return printStatement();
  else if (match(TokenType::RETURN)) {
    return returnStatement();
  } else if (match(TokenType::WHILE)) {
    return whileStatement();
  } else if (match(TokenType::LEFT_BRACE))
    return std::make_unique<BlockStatement>(block());
  else
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

Stmt
Parser::ifStatement()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  auto condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after 'if' condition");

  auto then_branch = statement();
  auto else_branch = Stmt{};
  if (match(TokenType::ELSE)) {
    else_branch = statement();
  }

  return std::make_unique<IfStatement>(
    std::move(condition), std::move(then_branch), std::move(else_branch));
}

Stmt
Parser::whileStatement()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'");
  auto condition = expression();

  consume(TokenType::RIGHT_PAREN, "Expect ')' after 'while' condition");
  auto body = statement();

  return std::make_unique<WhileStatement>(std::move(condition),
                                          std::move(body));
}

Stmt
Parser::forStatement()
{
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'");

  auto initializer = Stmt{};
  if (!match(TokenType::SEMICOLON)) {
    if (match(TokenType::VAR)) {
      initializer = varDeclaration();
    } else {
      initializer = expressionStatement();
    }
  }

  auto condition = Expr{};
  if (!check(TokenType::SEMICOLON)) {
    // there is a condition in front of ';'
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after loop condition");

  auto incr = Expr{};
  if (!check(TokenType::RIGHT_PAREN)) {
    incr = expression();
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");

  auto body = statement();

  if (incr) {
    /**
     * add incr as statement after rest of body
     * */
    auto block_body = std::vector<Stmt>{};
    block_body.push_back(std::move(body));
    block_body.push_back(
      std::make_unique<ExpressionStatement>(std::move(incr)));

    body = std::make_unique<BlockStatement>(std::move(block_body));
  }

  if (!condition) {
    condition = std::make_unique<LiteralExpression>(Object{ true });
  }
  body =
    std::make_unique<WhileStatement>(std::move(condition), std::move(body));

  if (initializer) {
    /**
     * initializer will only be run once as second statement
     * added to block_body contains the loop.
     * */
    auto block_body = std::vector<Stmt>{};
    block_body.push_back(std::move(initializer));
    block_body.push_back(std::move(body));
    body = std::make_unique<BlockStatement>(std::move(block_body));
  }

  return body;
}

Expr
Parser::expression()
{
  return assignment();
}

Expr
Parser::assignment()
{
  auto expr = orExpr();

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

  return call();
}

Expr
Parser::call()
{
  auto expr = primary();

  while (match(TokenType::LEFT_PAREN)) {
    expr = finishCall(std::move(expr));
  }

  return expr;
}

Expr
Parser::finishCall(Expr callee)
{
  auto args = std::vector<Expr>{};
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      args.push_back(expression());
    } while (match(TokenType::COMMA));
  }

  auto paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments");

  return std::make_unique<CallExpression>(std::move(callee), std::move(args));
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

Expr
Parser::orExpr()
{
  auto expr = andExpr();

  while (match(TokenType::OR)) {
    auto op = previous();
    auto rhs = equality();
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
}

Expr
Parser::andExpr()
{
  auto expr = equality();

  while (match(TokenType::AND)) {
    auto op = previous();
    auto rhs = equality();
    expr =
      std::make_unique<BinaryExpression>(std::move(expr), op, std::move(rhs));
  }

  return expr;
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