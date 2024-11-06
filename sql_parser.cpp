#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include <sstream>
#include "header.h"
#include "rapidcsv.hpp"

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

void crossJoin(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& tableName, LinkedList& columnsFromQuery){
    for (int i = 0; i < fileCountFirstTable; i++){ // пройдемся по всем файлам первой таблицы
        DBtable& firstTable = reinterpret_cast<DBtable&>(dbManager.tables.head->next->data); // приводим к типу DBtable
        string currentTable1 = firstTable.tableName; // получаем имя таблицы

        string tableDir1 = dbManager.schemaName + "/" + currentTable1 + "/" + currentTable1 + "_" + std::to_string(i + 1) + ".csv";
        string column1 = columnsFromQuery.head->data;
        
        rapidcsv::Document document1(tableDir1); // открываем файл 1
        int indexFirstColumn = document1.GetColumnIdx(column1); // считываем индекс искомой колонки 1
        int amountRow1 = document1.GetRowCount(); // считываем количество строк в файле 1
        cout << "Table1 has " << amountRow1 << " rows" << endl;
        for (int j = 0; j < amountRow1; j++){ // проходимся по всем строкам
            for (int k = 0; k < fileCountSecondTable; k++){ // пройдемся по второй таблице
                DBtable& secondTable = reinterpret_cast<DBtable&>(dbManager.tables.head->data); // приводим к типу DBtable
                string currentTable2 = secondTable.tableName; // получаем имя таблицы
                
                string tableDir2 = dbManager.schemaName + "/" + currentTable2 + "/" + currentTable2 + "_" + std::to_string(i + 1) + ".csv";
                string column2 = columnsFromQuery.head->next->data;
                rapidcsv::Document document2(tableDir2); // открываем файл 2
                int indexSecondColumn = document2.GetColumnIdx(column2); // считываем индекс искомой колонки 2
                int amountRow2 = document2.GetRowCount(); // считываем количество строк в файле 2
                cout << "Table2 has " << amountRow2 << " rows" << endl;
                for (int p = 0; p < amountRow2; ++p) {
                    if (indexFirstColumn < document1.GetColumnCount()) {
                        cout << document1.GetCell<string>(indexFirstColumn, j) << "  |   ";
                    } else {
                        cerr << "Column index out of bounds for first table" << endl;
                    }

                    if (indexSecondColumn < document2.GetColumnCount()) {
                        cout << document2.GetCell<string>(indexSecondColumn, p) << endl;
                    } else {
                        cerr << "Column index out of bounds for second table" << endl;
                    }
                }
            }
        }
    }
}

bool isLocked(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt";
        
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
    }
    string current; // чтение текущего значения блокировки
    file >> current;
    file.close();
    if (current == "locked") {
        return true; // заблокирована
    }
    return false; // разблокирована
}

void locking(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt"; 
    // блокируем через обновление txt файла
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
    }
    file << "locked";
    file.close();
}

void unlocking(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt";
    // а тут разблокируем, тоже с помощью файла
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
    }
    file << "unlocked";
    file.close();
}

void insertFunc(const DatabaseManager& dbManager, const std::string& tableName, string& query, int& currentKey){
    int number = 1;
    while (true) {
        string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(number) + ".csv";
        ifstream file(tableDir);
        if (!file.is_open()){
            cerr << "Error while reading file at" << tableDir << endl;
            return;
        }
        rapidcsv::Document doc(tableDir); // тут с помощью сторонней библиотеки считываем количество строк
        if (doc.GetRowCount() < dbManager.tuplesLimit) { // если количество строк меньше лимита
            break; // то другой таблицы нет, можно закончить цикл
        }
        number++; // в противном случае будем дальше идти по файлам
    }
    
    string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(number) + ".csv";
    // rapidcsv::Document doc(tableDir); // считываем содержимое файла
    // нужно реализовать проверку на ограничение tuplesLimit, поэтому считывание содержимого пока пусть тут будет на всякий
    ofstream csv(tableDir, ios::app); // ios::app чтобы добавлять в конец документа
    if (!csv.is_open()) { 
        cerr << "Не удалось открыть файл.\n";
        return;
    }
    
    csv << currentKey << ","; // пишем первичный ключ

    string value;
    bool insideQuotes = false;
    string currentValue;
    for (size_t i = 0; i < query.size(); i++) {
        char ch = query[i];
        // если встречаем одиночную кавычку, то меняем флаг
        if (ch == '\'') {
            insideQuotes = !insideQuotes;
            if (!insideQuotes && !currentValue.empty()) {
                // если закрыли кавычки, сохраняем значение в csv
                csv << currentValue;
                currentValue.clear();
                if (i + 1 < query.size() && query[i + 1] != ')') {
                    csv << ","; // если не последняя колонка, добавляем запятую
                } else {
                    csv << endl; // если последняя, то переносим на новую строку
                }
            }
        } else if (insideQuotes) {
            // если внутри кавычек, собираем значение дальше
            currentValue += ch;
        }
    }
    csv.close();

    // обновляем ключ
    currentKey++;
    ofstream newKeyFile(dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "pk_sequence.txt");
    if (!newKeyFile.is_open()){
        cerr << "Error while updating key file" << endl;
    }
    newKeyFile << currentKey; // перезаписываем ключ
    newKeyFile.close();
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

                iss >> wordFromQuery; // table1.column1 
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery, dbManager);
                int fileCountFirstTable = amountOfCSV(dbManager, tablesFromQuery.head->data);
                iss >> wordFromQuery; // table2.column1
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery, dbManager);
                int fileCountSecondTable = amountOfCSV(dbManager, tablesFromQuery.head->data);
                cout << tablesFromQuery.head->data << endl;

                crossJoin(fileCountFirstTable, fileCountSecondTable, dbManager, tablesFromQuery.head->data, columnsFromQuery);
                // Не все реализовано, но не могу проверить из-за отсутствия INSERT
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
                iss >> wordFromQuery;
                if (wordFromQuery != "INTO") {
                    cerr << "Incorrect command" << endl;
                }
                string tableName;
                iss >> tableName; // таблица1
                // нужна проверка на существование таблицы !!!!!!!
            
                if (isLocked(dbManager, tableName)){
                    cerr << "Table is locked" << endl;
                }
                locking(dbManager, tableName); // тут блокируем доступ к таблице
                int currentKey;
                string PKFile = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "pk_sequence.txt";
                ifstream keyFile(PKFile);
                if (!keyFile.is_open()) {
                    cerr << "Error while reading key file" << endl;
                }
                keyFile >> currentKey;
                keyFile.close();
                
                string query;
                iss >> wordFromQuery; // ('somedata',
                query += wordFromQuery;
                iss >> wordFromQuery; //  '12345')
                query += wordFromQuery; // ('somedata','12345')
                
                cout << query << endl;
                insertFunc(dbManager, tableName, query, currentKey); // тут функция вставки
                unlocking(dbManager, tableName); // а тут обратно разблокируем после произведения вставки
                
            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else {
            cerr << "Incorrect SQL query" << endl;
        }
    }
}
