#pragma once
#include <string>
#include <vector>
#include <cctype>
#include <unordered_map>

using namespace std;

enum class TokenType {
    KEYWORD, IDENT, NUMBER, OPERATOR, LPAREN, RPAREN,
    LBRACE, RBRACE, SEMICOLON, END, UNKNOWN
};

struct Token {
    TokenType type;
    string value;
};

class Lexer {
public:
    explicit Lexer(const string& src);
    vector<Token> tokenize();
private:
    string source;
    size_t pos = 0;
    char peek() const { return pos < source.size() ? source[pos] : '\0'; }
    char get() { return pos < source.size() ? source[pos++] : '\0'; }
    void skipWhitespace();
};