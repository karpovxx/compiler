#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

enum class TokenType {
    KEYWORD, IDENT, NUMBER, OPERATOR, LPAREN, RPAREN,
    LBRACE, RBRACE, SEMICOLON, END, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
public:
    explicit Lexer(const std::string& src);
    std::vector<Token> tokenize();
private:
    std::string source;
    size_t pos = 0;
    char peek() const { return pos < source.size() ? source[pos] : '\0'; }
    char get() { return pos < source.size() ? source[pos++] : '\0'; }
    void skipWhitespace();
};