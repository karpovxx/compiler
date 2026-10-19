#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "TAC.h"
using namespace std;

int main() {
    string source =
        "int a = 2 + 3 * 4;\n"
        "int b = a + 0;\n"
        "int c = b;\n";

    cout << "Исходный код:\n" << source << "\n";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    unique_ptr<ASTNode> ast;
    try {
        ast = parser.parseProgram();
        cout << "\n--- Дерево программы (AST) ---\n";
        ast->print();
    } catch (const exception& e) {
        cerr << "Ошибка парсинга: " << e.what() << "\n";
        return 1;
    }

    TACGenerator tacGen;
    if (ast) tacGen.generate(ast.get(), "a");

    cout << "\n--- ТАК до оптимизации ---\n";
    tacGen.print();

    tacGen.optimize();
    cout << "\n--- ТАК после оптимизации ---\n";
    tacGen.print();

    return 0;
}