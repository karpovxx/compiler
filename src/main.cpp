#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../include/Lexer.h"
#include "../include/Parser.h"
#include "../include/TAC.h"
using namespace std;

int main() {
    // Читаем файл из корня проекта (../input.txt)
    // Это нужно, чтобы программа работала, даже если ее запустить из папки bin
    ifstream inputFile("../input.txt"); 
    
    // Если не нашли в ../, пробуем в текущей (на случай запуска из корня)
    if (!inputFile.is_open()) {
        inputFile.open("input.txt");
    }
    
    if (!inputFile.is_open()) {
        cout << "Ошибка: не удалось открыть файл input.txt" << endl;
        cout << "Создайте файл input.txt в корне проекта и напишите там код на C." << endl;
        return 1;
    }

    stringstream buffer;
    buffer << inputFile.rdbuf();
    string source = buffer.str();
    inputFile.close();

    if (source.empty()) {
        cout << "Файл input.txt пуст. Напишите туда код на C." << endl;
        return 1;
    }

    cout << "--- Исходный код (из input.txt) ---\n" << source << "\n";

    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    vector<unique_ptr<ASTNode>> astList;
    
    try {
        astList = parser.parseProgram();
        cout << "--- Дерево программы (AST) ---\n";
        for (const auto& node : astList) {
            node->print();
        }
    } catch (const exception& e) {
        cerr << "\nОшибка парсинга: " << e.what() << "\n";
        return 1;
    }

    TACGenerator tacGen;
    for (const auto& node : astList) {
        tacGen.generate(node.get(), "");
    }

    cout << "\n--- ТАК до оптимизации ---\n";
    tacGen.print();

    tacGen.optimize();
    cout << "\n--- ТАК после оптимизации ---\n";
    tacGen.print();

    return 0;
}