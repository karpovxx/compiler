#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "../include/AST.h"

using namespace std;

struct TAC {
    string op, arg1, arg2, result;
    void print() const {
        if (op == "=") cout << result << " = " << arg1 << "\n";
        else if (op == "if") cout << "if " << arg1 << " goto " << result << "\n";
        else cout << result << " = " << arg1 << " " << op << " " << arg2 << "\n";
    }
};

class TACGenerator {
public:
    vector<TAC> code;
    int tempCounter = 0;
    string newTemp() { return "t" + to_string(++tempCounter); }
    string generate(const ASTNode* node, string target = "");
    void optimize();
    void print() const;
};