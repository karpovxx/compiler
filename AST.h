#pragma once
#include <string>
#include <memory>
#include <iostream>
#include <vector>

struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

struct NumberNode : ASTNode {
    std::string value;
    explicit NumberNode(std::string v) : value(std::move(v)) {}
    void print(int indent = 0) const override {
        using namespace std;
        cout << string(indent, ' ') << "Number(" << value << ")\n";
    }
};

struct VarNode : ASTNode {
    std::string name;
    explicit VarNode(std::string n) : name(std::move(n)) {}
    void print(int indent = 0) const override {
        using namespace std;
        cout << string(indent, ' ') << "Var(" << name << ")\n";
    }
};

struct BinaryOpNode : ASTNode {
    std::string op;
    std::unique_ptr<ASTNode> left, right;
    BinaryOpNode(std::string o, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
        : op(std::move(o)), left(std::move(l)), right(std::move(r)) {}
    void print(int indent = 0) const override {
        using namespace std;
        cout << string(indent, ' ') << "BinaryOp(" << op << ")\n";
        left->print(indent + 2);
        right->print(indent + 2);
    }
};

struct AssignNode : ASTNode {
    std::string var;
    std::unique_ptr<ASTNode> expr;
    AssignNode(std::string v, std::unique_ptr<ASTNode> e) : var(std::move(v)), expr(std::move(e)) {}
    void print(int indent = 0) const override {
        using namespace std;
        cout << string(indent, ' ') << "Assign(" << var << ")\n";
        expr->print(indent + 2);
    }
};

struct IfNode : ASTNode {
    std::unique_ptr<ASTNode> cond;
    std::vector<std::unique_ptr<ASTNode>> thenBody, elseBody;
    void print(int indent = 0) const override {
        using namespace std;
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