#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include <sstream>
#include "header.h"

using namespace std;

bool columnExists(const LinkedList& columnsFromQuery, const std::string& columnName) {
    
    return false; // Колонка не найдена
}


void splitPoint(LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, std::string& wordFromQuery, const DatabaseManager& dbManager) {
    // Удаляем пробелы в начале и конце
    wordFromQuery.erase(0, wordFromQuery.find_first_not_of(" "));
    wordFromQuery.erase(wordFromQuery.find_last_not_of(" ") + 1);

    // Найдем позицию точки
    size_t dotPos = wordFromQuery.find('.');
    if (dotPos != std::string::npos) {
        tablesFromQuery.addToTheHead(wordFromQuery.substr(0, dotPos)); // Сохраняем имя таблицы
        columnsFromQuery.addToTheHead(wordFromQuery.substr(dotPos + 1)); // Сохраняем имя колонки
    } else {
        std::cout << "Incorrect format: " << wordFromQuery << std::endl;
        return;
    }

    // Проверяем существование таблицы
    UniversalNode* current = dbManager.tables.head;
    bool tableExists = false;
    DBtable* currentTablePtr = nullptr;

    while (current != nullptr) {
        DBtable& currentTable = reinterpret_cast<DBtable&>(current->data); // Приведение к типу DBtable
        if (currentTable.tableName == tablesFromQuery.head->data) { // Сравниваем имя таблицы
            tableExists = true;
            currentTablePtr = &currentTable; // Сохраняем указатель на найденную таблицу
            break; // Таблица найдена
        }
        current = current->next; // Переход к следующему узлу
    }

    if (!tableExists) {
        std::cerr << "There is no such table" << std::endl;
        return;
    }

    // bool columnExists = false;
    // НУЖНО ДОБАВИТЬ ПРОВЕРКУ НА СУЩЕСТВОВАНИЕ КОЛОНОК!
    // if (!columnExists) {
    //     std::cerr << "There is no such column" << std::endl;
    //     return;
    // }
}

namespace fs = std::filesystem;

int amountOfCSV(const DatabaseManager& dbManager, const std::string& tableName) {
    int amount = 0; // ищем количество созданных csv файлов
    std::string tableDir;
    while (true) {
        tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(amount + 1) + ".csv";
        
        std::ifstream file(tableDir);
        if (!file.is_open()) { // если файл нельзя открыть, его нет
            break;
        }
        amount++;
        file.close();
    }
    return amount; // Возвращаем количество найденных файлов
}


void crossJoin(){
    
}

void QueryManager(const DatabaseManager& dbManager, DBtable& table) {
    string command;
    while(true){
        cout << "< ";
        getline(cin, command);
        istringstream iss(command);
        string wordFromQuery;
        iss >> wordFromQuery; // первое слово в команде
         
        if (wordFromQuery == "exit"){
            return;
        } else if (wordFromQuery == "SELECT"){
            try {
                LinkedList tablesFromQuery;
                LinkedList columnsFromQuery;
                iss >> wordFromQuery; // таблица1.колонка1
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery, dbManager);
                //DBtable& currentTable = reinterpret_cast<DBtable&>(current->data); // Приведение к типу DBtable
        
                int fileCount = amountOfCSV(dbManager, tablesFromQuery.head->data);
                cout << fileCount << endl;
                iss >> wordFromQuery; // таблица2.колонка1
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery, dbManager);
                // DBtable& currentTable = reinterpret_cast<DBtable&>(current->data); // Приведение к типу DBtable
                fileCount = amountOfCSV(dbManager, tablesFromQuery.head->data);
                cout << fileCount << endl;
                // cout << amountOfCSV(dbManager, currentTable) << endl;
                

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "DELETE"){
            try {
                cout << "del" << endl; 

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "INSERT"){
            try {
                cout << "ins" << endl;

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else {
            cerr << "Incorrect SQL query" << endl;
        }
    }
}
