#include "Parser.h"
#include <stdexcept>
using namespace std;

Parser::Parser(vector<Token> t) : tokens(move(t)) {}

bool Parser::match(TokenType type, const string& val) {
    if (peek().type == type && (val.empty() || peek().value == val)) {
        consume(); return true;
    }
    return false;
}

void Parser::expect(TokenType type, const string& val) {
    if (!match(type, val)) throw runtime_error("Syntax error at token: " + peek().value);
}

vector<unique_ptr<ASTNode>> Parser::parseProgram() {
    vector<unique_ptr<ASTNode>> stmts;
    while (peek().type != TokenType::END) {
        stmts.push_back(parseStatement());
    }
    return stmts;
}

unique_ptr<ASTNode> Parser::parseStatement() {
    if (peek().type == TokenType::KEYWORD && peek().value == "if") {
        return parseIf();
    }

    if (peek().type == TokenType::KEYWORD && peek().value == "int") {
        consume();
    }

    if (peek().type != TokenType::IDENT) {
        throw runtime_error("Expected variable name, got: " + peek().value);
    }
    string varName = consume().value;
    
    expect(TokenType::OPERATOR, "=");
    auto expr = parseExpression();
    expect(TokenType::SEMICOLON);
    
    return make_unique<AssignNode>(varName, move(expr));
}

unique_ptr<ASTNode> Parser::parseIf() {
    auto node = make_unique<IfNode>();
    consume();
    expect(TokenType::LPAREN);
    node->cond = parseExpression();
    expect(TokenType::RPAREN);
    expect(TokenType::LBRACE);
    while (peek().type != TokenType::RBRACE) {
        node->thenBody.push_back(parseStatement());
    }
    expect(TokenType::RBRACE);
    if (peek().type == TokenType::KEYWORD && peek().value == "else") {
        consume(); expect(TokenType::LBRACE);
        while (peek().type != TokenType::RBRACE) {
            node->elseBody.push_back(parseStatement());
        }
        expect(TokenType::RBRACE);
    }
    return node;
}

unique_ptr<ASTNode> Parser::parseExpression() {
    auto left = parseTerm();
    while (peek().type == TokenType::OPERATOR &&
          (peek().value == "+" || peek().value == "-" || peek().value == "==" ||
           peek().value == "<" || peek().value == ">")) {
        string op = consume().value;
        auto right = parseTerm();
        left = make_unique<BinaryOpNode>(op, move(left), move(right));
    }
    return left;
}

unique_ptr<ASTNode> Parser::parseTerm() {
    auto left = parseFactor();
    while (peek().type == TokenType::OPERATOR && (peek().value == "*" || peek().value == "/")) {
        string op = consume().value;
        auto right = parseFactor();
        left = make_unique<BinaryOpNode>(op, move(left), move(right));
    }
    return left;
}

unique_ptr<ASTNode> Parser::parseFactor() {
    if (peek().type == TokenType::NUMBER) return make_unique<NumberNode>(consume().value);
    if (peek().type == TokenType::IDENT) return make_unique<VarNode>(consume().value);
    if (match(TokenType::LPAREN)) {
        auto expr = parseExpression();
        expect(TokenType::RPAREN);
        return expr;
    }
    throw runtime_error("Unexpected token in factor: " + peek().value);
}