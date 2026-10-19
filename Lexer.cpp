#include "Lexer.h"
using namespace std;

Lexer::Lexer(const string& src) : source(src) {}

void Lexer::skipWhitespace() {
    while (pos < source.size() && isspace(source[pos])) pos++;
}

vector<Token> Lexer::tokenize() {
    vector<Token> tokens;
    unordered_map<string, TokenType> keywords = {
        {"int", TokenType::KEYWORD}, {"if", TokenType::KEYWORD},
        {"else", TokenType::KEYWORD}, {"while", TokenType::KEYWORD},
        {"return", TokenType::KEYWORD}
    };

    while (true) {
        skipWhitespace();
        char c = peek();
        if (c == '\0') { tokens.push_back({TokenType::END, ""}); break; }

        if (isalpha(c)) {
            string id;
            while (isalnum(peek())) id += get();
            if (keywords.count(id)) tokens.push_back({TokenType::KEYWORD, id});
            else tokens.push_back({TokenType::IDENT, id});
        } else if (isdigit(c)) {
            string num;
            while (isdigit(peek())) num += get();
            tokens.push_back({TokenType::NUMBER, num});
        } else if (c == '(') { tokens.push_back({TokenType::LPAREN, "("}); get(); }
        else if (c == ')') { tokens.push_back({TokenType::RPAREN, ")"}); get(); }
        else if (c == '{') { tokens.push_back({TokenType::LBRACE, "{"}); get(); }
        else if (c == '}') { tokens.push_back({TokenType::RBRACE, "}"}); get(); }
        else if (c == ';') { tokens.push_back({TokenType::SEMICOLON, ";"}); get(); }
        else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '=' || c == '<' || c == '>') {
            string op(1, c); get();
            if (c == '=' && peek() == '=') { op += get(); }
            tokens.push_back({TokenType::OPERATOR, op});
        } else {
            tokens.push_back({TokenType::UNKNOWN, string(1, c)}); get();
        }
    }
    return tokens;
}