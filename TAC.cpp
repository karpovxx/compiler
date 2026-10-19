#include "TAC.h"
#include "AST.h"
#include <cctype>
using namespace std;

void TACGenerator::generate(const ASTNode* node, string target) {
    if (auto num = dynamic_cast<const NumberNode*>(node)) {
        code.push_back({"=", num->value, "", target});
    } else if (auto var = dynamic_cast<const VarNode*>(node)) {
        code.push_back({"=", var->name, "", target});
    } else if (auto bin = dynamic_cast<const BinaryOpNode*>(node)) {
        string leftT = newTemp();
        string rightT = newTemp();
        generate(bin->left.get(), leftT);
        generate(bin->right.get(), rightT);
        code.push_back({bin->op, leftT, rightT, target});
    } else if (auto assign = dynamic_cast<const AssignNode*>(node)) {
        generate(assign->expr.get(), assign->var);
    }
}

void TACGenerator::optimize() {
    // Свертка констант
    for (auto& inst : code) {
        if (inst.op == "+" || inst.op == "-" || inst.op == "*" || inst.op == "/") {
            bool isNum1 = !inst.arg1.empty() && isdigit(inst.arg1[0]);
            bool isNum2 = !inst.arg2.empty() && isdigit(inst.arg2[0]);
            if (isNum1 && isNum2) {
                int v1 = stoi(inst.arg1), v2 = stoi(inst.arg2), res = 0;
                if (inst.op == "+") res = v1 + v2;
                else if (inst.op == "-") res = v1 - v2;
                else if (inst.op == "*") res = v1 * v2;
                else if (inst.op == "/" && v2 != 0) res = v1 / v2;
                inst.op = "=";
                inst.arg1 = to_string(res);
                inst.arg2 = "";
            }
        }
    }

    // Удаление мертвого кода
    vector<TAC> cleaned;
    for (size_t i = 0; i < code.size(); ++i) {
        bool used = false;
        for (size_t j = i + 1; j < code.size(); ++j) {
            if (code[j].arg1 == code[i].result || code[j].arg2 == code[i].result) {
                used = true; break;
            }
        }
        if (used || i == code.size() - 1) cleaned.push_back(code[i]);
    }
    code = cleaned;
}

void TACGenerator::print() const {
    cout << "--- Трехадресный код ---\n";
    for (const auto& inst : code) inst.print();
}