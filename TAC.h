#pragma once
#include "AST.h"
#include <string>
#include <vector>
#include <iostream>

struct TAC {
    std::string op, arg1, arg2, result;
    void print() const {
        using namespace std;
        if (op == "=") cout << result << " = " << arg1 << "\n";
        else if (op == "if") cout << "if " << arg1 << " goto " << result << "\n";
        else cout << result << " = " << arg1 << " " << op << " " << arg2 << "\n";
    }
};

class TACGenerator {
public:
    std::vector<TAC> code;
    int tempCounter = 0;
    std::string newTemp() { return "t" + std::to_string(++tempCounter); }
    void generate(const ASTNode* node, std::string target = "");
    void optimize();
    void print() const;
};