#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include <sstream>
#include "header.h"

using namespace std;
namespace fs = std::filesystem;

bool columnExists(const LinkedList& columnsFromQuery, const std::string& columnName) { // нужно сделать
    
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

    // проверяем существование таблицы
    UniversalNode* current = dbManager.tables.head; // заходим в дб менеджер, где хранятся все названия
    bool tableExists = false; // флаг
    DBtable* currentTablePtr = nullptr;

    while (current != nullptr) {
        DBtable& currentTable = reinterpret_cast<DBtable&>(current->data); // приведение к типу DBtable
        if (currentTable.tableName == tablesFromQuery.head->data) { // сравниваем имя таблицы
            tableExists = true;
            currentTablePtr = &currentTable; // сохраняем указатель на найденную таблицу
            break; // таблица найдена, дальше идти не нужно
        }
        current = current->next; // переходим к следующему узлу
    }

    if (!tableExists) {
        std::cerr << "There is no such table" << std::endl;
        return;
    }

    // НУЖНО ДОБАВИТЬ ПРОВЕРКУ НА СУЩЕСТВОВАНИЕ КОЛОНОК!
    // if (!columnExists) {
    //     std::cerr << "There is no such column" << std::endl;
    //     return;
    // }
}

int amountOfCSV(const DatabaseManager& dbManager, const std::string& tableName) {
    int amount = 0; // ищем количество созданных csv файлов
    std::string tableDir;
    while (true) {
        tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(amount + 1) + ".csv";
        
        std::ifstream file(tableDir);
        if (!file.is_open()) { // если файл нельзя открыть, то он не существует
            break;
        }
        amount++;
        file.close();
    }
    return amount; // возвращаем количество найденных файлов
}


void crossJoin(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& tableName){
    for (int i = 1; i < fileCountFirstTable; i++){ // пройдемся по всем файлам первой таблицы
        string tableDir1 = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(i + 1) + ".csv";
        // для правильности надо обрезать будет, потому что будет работать только для первой таблицы, а не для второй
        // либо можно попробовать не обрезать, а использовать некст значение в списке, чтобы прийти ко второму записанному (звучит хорошо)
        
        for (int j = 1; j < fileCountSecondTable; j++){ // пройдемся по второй таблице

        }
    }
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
                int fileCountFirstTable = amountOfCSV(dbManager, tablesFromQuery.head->data);
                iss >> wordFromQuery; // таблица2.колонка1
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery, dbManager);
                int fileCountSecondTable = amountOfCSV(dbManager, tablesFromQuery.head->data);

                crossJoin(fileCountFirstTable, fileCountSecondTable, dbManager, tablesFromQuery.head->data);

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
