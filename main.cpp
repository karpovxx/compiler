#include <iostream>
#include <fstream>
#include <sstream>
#include "Lexer.h"
#include "Parser.h"
#include "TAC.h"
using namespace std;

int main() {
    // Открываем файл с исходным кодом
    ifstream inputFile("input.txt");
    
    if (!inputFile.is_open()) {
        cout << "Ошибка: не удалось открыть файл input.txt" << endl;
        cout << "Создайте файл input.txt в папке с программой и напишите там код на C." << endl;
        return 1;
    }

    // Читаем весь файл в строку
    stringstream buffer;
    buffer << inputFile.rdbuf();
    string source = buffer.str();
    inputFile.close();

    if (source.empty()) {
        cout << "Файл input.txt пуст. Напишите туда код на C." << endl;
        return 1;
    }

    cout << "--- Исходный код (из input.txt) ---\n" << source << "\n";

    // 1. Лексический анализ
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    // 2. Синтаксический анализ
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

    // 3. Генерация ТАК
    TACGenerator tacGen;
    for (const auto& node : astList) {
        tacGen.generate(node.get(), "");
    }

    cout << "\n--- ТАК до оптимизации ---\n";
    tacGen.print();

    // 4. Оптимизация
    tacGen.optimize();
    cout << "\n--- ТАК после оптимизации ---\n";
    tacGen.print();

    cout << "\nНажмите Enter для выхода...";
    cin.get(); // Чтобы консоль не закрывалась сразу
    return 0;
}