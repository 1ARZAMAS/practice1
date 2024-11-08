#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include <thread>
#include <sstream>
#include "header.h"
#include "rapidcsv.hpp"

using namespace std;
namespace fs = std::filesystem;

bool tableExists(const DatabaseManager& dbManager, const std::string& tableName) {
    UniversalNode* current = dbManager.tables.head;
    while (current != nullptr) { // пройдемся по списку таблиц
        DBtable& currentTable = reinterpret_cast<DBtable&>(current->data);
        if (currentTable.tableName == tableName) { // если значение нашлось, таблица существует
            return true;
        }
        current = current->next;
    }
    return false;
}

void splitPoint(LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, std::string& wordFromQuery) {
    size_t dotPos = wordFromQuery.find('.'); // найдем позицию точки
    if (dotPos != std::string::npos) {
        tablesFromQuery.addToTheHead(wordFromQuery.substr(0, dotPos)); // Сохраняем имя таблицы
        columnsFromQuery.addToTheHead(wordFromQuery.substr(dotPos + 1)); // Сохраняем имя колонки
    } else {
        std::cout << "Incorrect format: " << wordFromQuery << std::endl;
        return;
    }
}

string cleanColumnName(const string& str) {
    string cleaned = str;
    if (!cleaned.empty() && cleaned.back() == ',') {
        cleaned.pop_back(); // убираем последнюю запятую
    }
    // убираем пробелы
    int start = cleaned.find_first_not_of(" \t");
    int end = cleaned.find_last_not_of(" \t");
    if (start == string::npos || end == string::npos) {
        return ""; // если только пробелы, вернем пустую строку
    }
    return cleaned.substr(start, end - start + 1);
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
        DBtable& firstTable = reinterpret_cast<DBtable&>(dbManager.tables.head->data); // приводим к типу DBtable
        string currentTable1 = firstTable.tableName; // получаем имя таблицы

        string tableDir1 = dbManager.schemaName + "/" + currentTable1 + "/" + currentTable1 + "_" + std::to_string(i + 1) + ".csv";
        string column1 = cleanColumnName(columnsFromQuery.head->next->data);
        
        rapidcsv::Document document1(tableDir1); // открываем файл 1
        int indexFirstColumn = document1.GetColumnIdx(column1); // считываем индекс искомой колонки 1
        int amountRow1 = document1.GetRowCount(); // считываем количество строк в файле 1
        for (int j = 0; j < amountRow1; j++){ // проходимся по всем строкам
            for (int k = 0; k < fileCountSecondTable; k++){ // пройдемся по второй таблице
                DBtable& secondTable = reinterpret_cast<DBtable&>(dbManager.tables.head->next->data); // приводим к типу DBtable
                string currentTable2 = secondTable.tableName; // получаем имя таблицы
                
                string tableDir2 = dbManager.schemaName + "/" + currentTable2 + "/" + currentTable2 + "_" + std::to_string(k + 1) + ".csv";
                string column2 = cleanColumnName(columnsFromQuery.head->data);
                rapidcsv::Document document2(tableDir2); // открываем файл 2

                int indexSecondColumn = document2.GetColumnIdx(column2); // считываем индекс искомой колонки 2
                int amountRow2 = document2.GetRowCount(); // считываем количество строк в файле 2
                for (int p = 0; p < amountRow2; ++p) {
                    // if (indexFirstColumn < document1.GetColumnCount()) {
                    //     cout << document1.GetCell<string>(indexFirstColumn, j) << "  |   ";
                    // } else {
                    //     cerr << "Column index out of bounds for first table" << endl;
                    // }
                    // if (indexSecondColumn < document2.GetColumnCount()) {
                    //     cout << document2.GetCell<string>(indexSecondColumn, p) << endl;
                    // } else {
                    //     cerr << "Column index out of bounds for second table" << endl;
                    // }
                    cout << document1.GetCell<string>(0, j) << ": ";
                    cout << document1.GetCell<string>(indexFirstColumn, j) << "  |   ";
                    cout << document2.GetCell<string>(0, p) << ": ";
                    cout << document2.GetCell<string>(indexSecondColumn, p) << endl;
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
        return true; // так как прочитать файл не смогли, вернем, что таблица заблокирована к редактированию
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
        return;
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
        return;
    }
    file << "unlocked";
    file.close();
}

void copyFirstRow(string& firstTable, string& tableDir){
    string firstRow;
    ifstream firstFile(firstTable); // откроем первую таблицу и считаем первую строку
    if (!firstFile.is_open()) {
        cerr << "Error while opening file" << endl;
        return;
    }
    firstFile >> firstRow;
    firstFile.close();
    ofstream secondFile(tableDir); // откроем вторую таблицу и запишем первую строку
    if (!secondFile.is_open()) {
        cerr << "Error while opening file" << endl;
        return;
    }
    secondFile << firstRow << endl;
    secondFile.close();
}

void insertFunc(const DatabaseManager& dbManager, const std::string& tableName, string& query, int& currentKey){
    int number = 1;
    while (true) { // для того чтобы понимать в какой именно файл нужно будет записывать данные, не заполнены ли остальные
        string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(number) + ".csv";
        ofstream file(tableDir, ios::app);
        if (!file.is_open()){
            cerr << "Error while reading file at" << tableDir << endl;
            return;
        }
        rapidcsv::Document doc(tableDir); // тут с помощью сторонней библиотеки считываем количество строк
        if (doc.GetRowCount() < dbManager.tuplesLimit) { // если количество строк меньше лимита
            break; // то останавливаем цикл, ибо будем записывать в эту таблицу
        }
        number++; // в противном случае будем дальше идти по файлам
    }
    string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(number) + ".csv";
    
    rapidcsv::Document doc(tableDir); // считываем содержимое файла
    if (doc.GetRowCount() == 0) { // если текущий файл пустой, запишем в него первую строку с колонками
        string firstTable = dbManager.schemaName + "/" + tableName + "/" + tableName + "_1.csv";
        copyFirstRow(firstTable, tableDir); // так как мы их считываем, чтобы корректно вставлять данные
    }
    
    fstream csv(tableDir); // ios::app чтобы добавлять в конец документа
    if (!csv.is_open()) { 
        cerr << "Error while opening the file" << endl;
        return;
    }
    
    bool insideQuotes = false;
    string currentValue;
    LinkedList dataList; // тут будут находится все значения, которые захотим записать
    for (int i = 0; i < query.size(); i++) {
        char ch = query[i];
        // если встречаем одиночную кавычку, то меняем флаг
        if (ch == '\'') {
            insideQuotes = !insideQuotes;
            if (!insideQuotes && !currentValue.empty()) {
                // если закрыли кавычки, сохраняем значение в dataList
                dataList.addToTheEnd(currentValue);
                currentValue.clear();
            }
        } else if (insideQuotes) {
            // если внутри кавычек, собираем значение дальше
            currentValue += ch;
        }
    }

    Node* current = dataList.head;
    int counter = 1; // начнем с 1, потому что первая колонка - это ключ
    while (current != nullptr){
        counter++;
        current = current->next;
    }
    
    string tempString;
    csv >> tempString;
    int tempCounter = 1;
    for(int i = 0; i < tempString.size(); i++){
        if (tempString[i] == ','){
            tempCounter++;
        }
    }

    if (tempCounter < counter){ // если количество записываемых значений больше, чем колонок, вернем ошибку
        cerr << "Error while inserting data: more values than columns" << endl;
        return;
    }
    csv.close();

    fstream csv1(tableDir, ios::app); // ios::app чтобы добавлять в конец документа
    if (!csv1.is_open()) { 
        cerr << "Error while opening the file" << endl;
        return;
    }

    csv1 << currentKey << ","; // пишем первичный ключ

    Node* currentData = dataList.head; // пройдемся по всем значениям
    while(currentData != nullptr){ // и запишем их
        csv1 << currentData->data;
        if (currentData->next != nullptr){ // если ссылка не на nullptr, 
            csv1 << ","; // значит, не конец списка, ставим запятую
        } else {
            csv1 << "\n"; // в противном случае просто перейдем на новую строку
        }
        currentData = currentData->next;
    }

    csv1.close();

    // обновляем ключ
    currentKey++;
    ofstream newKeyFile(dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "pk_sequence.txt");
    if (!newKeyFile.is_open()){
        cerr << "Error while updating key file" << endl;
    }
    newKeyFile << currentKey; // перезаписываем ключ
    newKeyFile.close();
}

void deleteFunc(const DatabaseManager& dbManager, const std::string& tableName, string& query, LinkedList& tableFromQuery, LinkedList& columnFromQuery){
    bool insideQuotes = false;
    string currentValue;
    LinkedList dataList; // тут будут находится все значения, которые захотим удалить
    for (int i = 0; i < query.size(); i++) {
        char ch = query[i];
        // если встречаем одиночную кавычку, то меняем флаг
        if (ch == '\'') {
            insideQuotes = !insideQuotes;
            if (!insideQuotes && !currentValue.empty()) {
                // если закрыли кавычки, сохраняем значение в dataList
                dataList.addToTheEnd(currentValue);
                currentValue.clear();
            }
        } else if (insideQuotes) {
            // если внутри кавычек, собираем значение дальше
            currentValue += ch;
        }
    }

    bool valueExists = false;
    Node* currentData = dataList.head; // пройдемся по всем значениям
    for(int i = 0; i < amountOfCSV(dbManager, tableName); i++){
        string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(i + 1) + ".csv";
        rapidcsv::Document doc(tableDir); // считываем содержимое файла
        int amountOfRows = doc.GetRowCount();
        while(currentData != nullptr){
            for (int j = 0; j < amountOfRows; j++){    
                // GetCell(колонка, строка)
                if (doc.GetCell<string>(columnFromQuery.head->data, j) == currentData->data){
                    valueExists = true;
                    doc.RemoveRow(j);
                    doc.Save(tableDir);
                    j--;
                    amountOfRows--;
                }
            }
            currentData = currentData->next;
        }
    }
    if (!valueExists){
        cerr << "Value does not exist" << endl;
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
        } else if (wordFromQuery == "SELECT"){ // требует дальнейшей реализации
            try {
                LinkedList tablesFromQuery;
                LinkedList columnsFromQuery;

                iss >> wordFromQuery; // table1.column1 
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery);
                int fileCountFirstTable = amountOfCSV(dbManager, tablesFromQuery.head->data);
                iss >> wordFromQuery; // table2.column1
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery);
                int fileCountSecondTable = amountOfCSV(dbManager, tablesFromQuery.head->data);

                crossJoin(fileCountFirstTable, fileCountSecondTable, dbManager, tablesFromQuery.head->data, columnsFromQuery);
                // Не все реализовано, проект собирается, но при этом выдает некорректные значения, + нужно доделать оставшуюся часть sql запроса
            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "DELETE"){
            try {
                // DELETE FROM таблица1 WHERE таблица1.колонка1 = '123'
                // обрабатываем запрос
                 
                iss >> wordFromQuery;
                if (wordFromQuery != "FROM") {
                    throw std::runtime_error("Incorrect command");
                }
                string tableName;
                iss >> tableName; // table1
                if (!tableExists(dbManager, tableName)) {
                    throw std::runtime_error("Table does not exist");
                }
                if (isLocked(dbManager, tableName)){
                    throw std::runtime_error("Table is locked");
                }

                iss >> wordFromQuery;
                if (wordFromQuery != "WHERE") {
                    throw std::runtime_error("Incorrect command");
                }
                iss >> wordFromQuery; // table1.column1 
                LinkedList tableFromQuery;
                LinkedList columnFromQuery;
                splitPoint(tableFromQuery, columnFromQuery, wordFromQuery);
                if (tableFromQuery.head->data != tableName){
                    throw runtime_error("Incorrect table in query");
                }

                iss >> wordFromQuery; // =
                if (wordFromQuery != "=") {
                    throw std::runtime_error("Incorrect command");
                }

                locking(dbManager, tableName); // тут блокируем доступ к таблице

                string query;
                string valuesPart;
                getline(iss, valuesPart); // считываем оставшуюся часть строки (вдруг захотим удалять не по одному значению)
                query += valuesPart;
                deleteFunc(dbManager, tableName, query, tableFromQuery, columnFromQuery); // тут функция удаления

                unlocking(dbManager, tableName); // а тут разблокируем после произведения удаления

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "INSERT"){
            try {
                // обрабатываем запрос
                iss >> wordFromQuery;
                if (wordFromQuery != "INTO") {
                    throw std::runtime_error("Incorrect command");
                }
                string tableName;
                iss >> tableName; // table1
                if (!tableExists(dbManager, tableName)) {
                    throw std::runtime_error("Table does not exist");
                }
                iss >> wordFromQuery;
                if (wordFromQuery != "VALUES") {
                    throw std::runtime_error("Incorrect command");
                }
                if (isLocked(dbManager, tableName)){
                    throw std::runtime_error("Table is locked");
                }
                locking(dbManager, tableName); // тут блокируем доступ к таблице

                int currentKey;
                string PKFile = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "pk_sequence.txt";
                ifstream keyFile(PKFile);
                if (!keyFile.is_open()) {
                    throw std::runtime_error("Error while reading key file");
                }
                keyFile >> currentKey;
                keyFile.close();
                
                string query;
                string valuesPart;
                getline(iss, valuesPart); // считываем оставшуюся часть строки 
                query += valuesPart;
                insertFunc(dbManager, tableName, query, currentKey); // тут функция вставки

                unlocking(dbManager, tableName); // а тут разблокируем после произведения вставки
                
            } catch (const exception& error) {
                cerr << error.what() << endl;
            }
        } else {
            cerr << "Incorrect SQL query" << endl;
        }
    }
}