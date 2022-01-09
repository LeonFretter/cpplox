#include <Scanner.hpp>
#include <array>
#include <map>

namespace Lox {

std::vector<Token>
Scanner::scanTokens()
{
  while (!isAtEnd()) {
    start = current;
    scanToken();
  }

  tokens.push_back(Token{ TokenType::END_OF_FILE, "" });
  return tokens;
}

Scanner::Scanner(size_t src_sz, char const* src)
  : src_sz(src_sz)
  , src(src)
  , tokens()
  , start(0UL)
  , current(0UL)
{}

Scanner::Scanner(std::vector<char> const& src)
  : Scanner(src.size(), src.data())
{}

Scanner::Scanner(std::string const& src)
  : Scanner(src.size(), src.c_str())
{}

void
Scanner::scanToken()
{
  auto c = src[current++];

  static constexpr auto one_char_tokens =
    std::array{ '(', ')', '{', '}', ',', '.', '-', '+', ';', '*' };

  if (isWhitespace(c)) {
    // whitespace
    return;

  } else if (std::find(one_char_tokens.begin(), one_char_tokens.end(), c) !=
             one_char_tokens.end()) {
    // one-char tokens
    addToken(oneCharTokenType(c));
  } else if (isDigit(c)) {
    //   number
    addNumber();
  } else if (isAlpha(c)) {
    //   identifier
    addIdentifier();
  } else {
    //   one- or two-char tokens
    switch (c) {
      case '!':
        addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        break;
      case '=':
        addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        break;
      case '<':
        addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::EQUAL);
        break;
      case '>':
        addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::EQUAL);
        break;
      case '/':
        if (match('/')) {
          // a comment goes until end of line
          while (peek() != '\n' && !isAtEnd())
            advance();
        } else {
          addToken(TokenType::SLASH);
        }
        break;

      // literals
      case '"':
        addString();
        break;

      default:
        throw new std::runtime_error("Unrecognized character");
    }
  }
}

char
Scanner::peek(size_t offset) const
{
  if (isAtEnd(offset)) {
    return '\0';
  } else {
    return src[current + offset];
  }
}

char
Scanner::advance()
{
  return src[current++];
}

void
Scanner::addToken(TokenType token_type)
{
  addToken(token_type, Object{});
}

void
Scanner::addToken(TokenType token_type, Object literal)
{
  tokens.push_back(Token{ token_type,
                          std::string(src + start, current - start),
                          std::move(literal) });
}

void
Scanner::addString()
{
  while (peek() != '"' && !isAtEnd()) {
    advance();
  }

  if (isAtEnd()) {
    throw std::runtime_error("Unterminated string.");
  }

  // end-quote
  advance();

  //   Trim surrounding quotes
  auto value = std::string(src + start + 1UL, current - start - 2UL);

  addToken(TokenType::STRING, Object{ value });
}

void
Scanner::addNumber()
{
  while (isDigit(peek()))
    advance();

  if (peek() == '.' && isDigit(peek(1UL))) {
    //   consume the "."
    advance();

    while (isDigit(peek()))
      advance();
  }

  auto value = std::stod(std::string{ src + start, current - start });
  addToken(TokenType::NUMBER, Object{ value });
}

void
Scanner::addIdentifier()
{
  while (isAlphaNumeric(peek()))
    advance();

  static auto keywords = std::map<std::string, TokenType>{
    { "and", TokenType::AND },       { "class", TokenType::CLASS },
    { "else", TokenType::ELSE },     { "false", TokenType::FALSE },
    { "for", TokenType::FOR },       { "fun", TokenType::FUN },
    { "if", TokenType::IF },         { "nil", TokenType::NIL },
    { "or", TokenType::OR },         { "print", TokenType::PRINT },
    { "return", TokenType::RETURN }, { "super", TokenType::SUPER },
    { "this", TokenType::THIS },     { "true", TokenType::TRUE },
    { "var", TokenType::VAR },       { "while", TokenType::WHILE }
  };

  auto identifier = std::string(src + start, current - start);

  if (keywords.contains(identifier)) {
    addToken(keywords.at(identifier));
  } else {
    addToken(TokenType::IDENTIFIER);
  }
}

bool
Scanner::match(char expected)
{
  if (isAtEnd()) {
    return false;
  } else {
    if (src[current] != expected)
      return false;
    else {
      ++current;
      return true;
    }
  }
}

TokenType
Scanner::oneCharTokenType(char c)
{
  auto token_map = std::map<char, TokenType>{
    { '(', TokenType::LEFT_PAREN }, { ')', TokenType::RIGHT_PAREN },
    { '{', TokenType::LEFT_BRACE }, { '}', TokenType::RIGHT_BRACE },
    { ',', TokenType::COMMA },      { '.', TokenType::DOT },
    { '-', TokenType::MINUS },      { '+', TokenType::PLUS },
    { ';', TokenType::SEMICOLON },  { '*', TokenType::STAR }
  };

  return token_map.at(c);
}

bool
Scanner::isWhitespace(char c)
{
  static constexpr auto whitespace_chars = std::array{ ' ', '\r', '\t', '\n' };
  return std::find(whitespace_chars.begin(), whitespace_chars.end(), c) !=
         whitespace_chars.end();
}

} // namespace Lox