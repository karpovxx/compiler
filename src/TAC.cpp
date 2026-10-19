#include "../include/TAC.h"
#include <cctype>
#include <map>
using namespace std;

string TACGenerator::generate(const ASTNode* node, string target) {
    if (auto num = dynamic_cast<const NumberNode*>(node)) {
        if (target.empty()) target = newTemp();
        code.push_back({"=", num->value, "", target});
        return target;
    } 
    else if (auto var = dynamic_cast<const VarNode*>(node)) {
        return var->name;
    } 
    else if (auto bin = dynamic_cast<const BinaryOpNode*>(node)) {
        string leftT = generate(bin->left.get());
        string rightT = generate(bin->right.get());
        if (target.empty()) target = newTemp();
        code.push_back({bin->op, leftT, rightT, target});
        return target;
    } 
    else if (auto assign = dynamic_cast<const AssignNode*>(node)) {
        string exprResult = generate(assign->expr.get());
        if (exprResult != assign->var) {
            code.push_back({"=", exprResult, "", assign->var});
        }
        return assign->var;
    }
    return "";
}

void TACGenerator::optimize() {
    map<string, string> constants;

    for (auto& inst : code) {
        if (inst.op == "=") {
            if (constants.count(inst.arg1)) inst.arg1 = constants[inst.arg1];
            bool isNum = !inst.arg1.empty() && (isdigit(inst.arg1[0]) || inst.arg1[0] == '-');
            if (isNum) constants[inst.result] = inst.arg1;
            else constants.erase(inst.result);
        } 
        else if (inst.op == "+" || inst.op == "-" || inst.op == "*" || inst.op == "/") {
            if (constants.count(inst.arg1)) inst.arg1 = constants[inst.arg1];
            if (constants.count(inst.arg2)) inst.arg2 = constants[inst.arg2];

            bool isNum1 = !inst.arg1.empty() && (isdigit(inst.arg1[0]) || inst.arg1[0] == '-');
            bool isNum2 = !inst.arg2.empty() && (isdigit(inst.arg2[0]) || inst.arg2[0] == '-');
            
            if (isNum1 && isNum2) {
                int v1 = stoi(inst.arg1), v2 = stoi(inst.arg2), res = 0;
                if (inst.op == "+") res = v1 + v2;
                else if (inst.op == "-") res = v1 - v2;
                else if (inst.op == "*") res = v1 * v2;
                else if (inst.op == "/" && v2 != 0) res = v1 / v2;
                
                inst.op = "=";
                inst.arg1 = to_string(res);
                inst.arg2 = "";
                constants[inst.result] = inst.arg1;
            }
        }
    }

    vector<TAC> cleaned;
    string lastResult = code.empty() ? "" : code.back().result;
    vector<bool> used(code.size(), false);
    if (!code.empty()) used[code.size() - 1] = true;

    for (int i = code.size() - 1; i >= 0; --i) {
        if (!used[i]) continue;
        for (int j = i - 1; j >= 0; --j) {
            if (code[j].result == code[i].arg1 || code[j].result == code[i].arg2) {
                used[j] = true;
            }
        }
    }

    for (size_t i = 0; i < code.size(); ++i) {
        if (used[i]) cleaned.push_back(code[i]);
    }
    code = cleaned;
}

void TACGenerator::print() const {
    if (code.empty()) cout << "(пусто)\n";
    for (const auto& inst : code) inst.print();
}