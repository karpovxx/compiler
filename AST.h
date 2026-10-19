#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <vector>

using namespace std;

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

struct NumberNode : ASTNode {
    string value;
    explicit NumberNode(string v) : value(v) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Number(" << value << ")\n";
    }
};

struct VarNode : ASTNode {
    string name;
    explicit VarNode(string n) : name(n) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Var(" << name << ")\n";
    }
};

struct BinaryOpNode : ASTNode {
    string op;
    unique_ptr<ASTNode> left, right;
    BinaryOpNode(string o, unique_ptr<ASTNode> l, unique_ptr<ASTNode> r)
        : op(o), left(move(l)), right(move(r)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "BinaryOp(" << op << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct AssignNode : ASTNode {
    string var;
    unique_ptr<ASTNode> expr;
    AssignNode(string v, unique_ptr<ASTNode> e) : var(v), expr(move(e)) {}
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "Assign(" << var << ")\n";
        expr->print(indent + 2);
    }
};

struct IfNode : ASTNode {
    unique_ptr<ASTNode> cond;
    vector<unique_ptr<ASTNode>> thenBody, elseBody;
    void print(int indent = 0) const override {
        cout << string(indent, ' ') << "If\n";
        cout << string(indent + 2, ' ') << "Condition:\n";
        cond->print(indent + 4);
        cout << string(indent + 2, ' ') << "Then:\n";
        for (auto& n : thenBody) n->print(indent + 4);
        if (!elseBody.empty()) {
            cout << string(indent + 2, ' ') << "Else:\n";
            for (auto& n : elseBody) n->print(indent + 4);
        }
    }
};