#pragma once
#include "../include/Lexer.h"
#include "../include/AST.h"
#include <memory>
#include <vector>

using namespace std;

class Parser {
public:
    explicit Parser(vector<Token> tokens);
    vector<unique_ptr<ASTNode>> parseProgram();
private:
    vector<Token> tokens;
    size_t pos = 0;
    Token peek() const { return tokens[pos]; }
    Token consume() { return tokens[pos++]; }
    bool match(TokenType type, const string& val = "");
    void expect(TokenType type, const string& val = "");
    unique_ptr<ASTNode> parseStatement();
    unique_ptr<ASTNode> parseExpression();
    unique_ptr<ASTNode> parseTerm();
    unique_ptr<ASTNode> parseFactor();
    unique_ptr<ASTNode> parseIf();
};