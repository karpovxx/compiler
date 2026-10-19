#pragma once
#include "Lexer.h"
#include "AST.h"
#include <memory>

class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    std::unique_ptr<ASTNode> parseProgram();
private:
    std::vector<Token> tokens;
    size_t pos = 0;
    Token peek() const { return tokens[pos]; }
    Token consume() { return tokens[pos++]; }
    bool match(TokenType type, const std::string& val = "");
    void expect(TokenType type, const std::string& val = "");
    std::unique_ptr<ASTNode> parseStatement();
    std::unique_ptr<ASTNode> parseExpression();
    std::unique_ptr<ASTNode> parseTerm();
    std::unique_ptr<ASTNode> parseFactor();
    std::unique_ptr<ASTNode> parseIf();
};