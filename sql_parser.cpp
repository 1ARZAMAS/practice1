#include "header.h"
#include "lock.h"
#include "functions.h"
#include "sqlFuncs.h"

using namespace std;
namespace fs = std::filesystem;

void QueryManager(const DatabaseManager& dbManager, DBtable& table) {
    string command;
    while(true){
        cout << "< ";
        getline(cin, command);
        istringstream iss(command);
        string wordFromQuery;
        if (!(iss >> wordFromQuery)){ // первое слово в команде
            throw runtime_error("Command is not complete");
        } 
         
        if (wordFromQuery == "exit"){
            return;
        } else if (wordFromQuery == "SELECT"){
            try {
                LinkedList tablesFromQuery;
                LinkedList columnsFromQuery;

                if (!(iss >> wordFromQuery)){ // table1.column1 
                    throw runtime_error("Data expected");
                } 
                if (findDot(wordFromQuery) == string::npos){
                    throw runtime_error("Incorrect data format: expected table1.column1");
                }
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery);
                if (!tablesFromQuery.head || !columnsFromQuery.head){
                    throw runtime_error("Invalid data");
                }
                int fileCountFirstTable = amountOfCSV(dbManager, tablesFromQuery.head->data);
                if (!(iss >> wordFromQuery)){ // table2.column1 
                    throw runtime_error("Data expected");
                } 
                if (findDot(wordFromQuery) == string::npos){
                    throw runtime_error("Incorrect data format: expected table1.column1");
                }
                splitPoint(tablesFromQuery, columnsFromQuery, wordFromQuery);
                if (!tablesFromQuery.head || !columnsFromQuery.head){
                    throw runtime_error("Invalid data");
                }
                int fileCountSecondTable = amountOfCSV(dbManager, tablesFromQuery.head->data);

                if (!(iss >> wordFromQuery) || wordFromQuery != "FROM") {
                    throw runtime_error("Expected 'FROM' keyword");
                }
                // проверка на то, что названия таблиц из table1.column1 будут такими же как и после FROM, те table1
                // (условно)
                string tableName;
                if (!(iss >> tableName)) {
                    throw runtime_error("Expected table name after 'FROM'");
                }
                string cleanTable = cleanString(tableName);
                Node* currentTable = tablesFromQuery.head;
                bool tableFound = false;
                while (currentTable != nullptr){
                    if (currentTable->data == cleanTable){
                        tableFound = true;
                        break;
                    }
                    currentTable = currentTable->next;
                }
                if (!tableFound){
                    throw runtime_error("Incorrect table in query");
                }
                if (!(iss >> tableName)) {
                    throw runtime_error("Expected second table name after 'FROM'");
                }
                cleanTable = cleanString(tableName);
                Node* currentSecondTable = tablesFromQuery.head;
                tableFound = false;
                while (currentSecondTable != nullptr){
                    if (currentSecondTable->data == cleanTable){
                        tableFound = true;
                        break;
                    }
                    currentSecondTable = currentSecondTable->next;
                }
                if (!tableFound){
                    throw runtime_error("Incorrect table in query");
                }

                string nextWord;
                iss >> nextWord;
                bool hasWhere = false;
                if (iss >> nextWord && nextWord == "WHERE"){ // проверим, есть ли следующее слово WHERE
                    hasWhere = true;
                }

                if (hasWhere) {
                    string query;
                    string valuesPart;
                    getline(iss, valuesPart); // считываем оставшуюся часть строки
                    if (valuesPart.empty()) {
                        throw runtime_error("Expected conditions after WHERE");
                    }
                    query += valuesPart; // table1.column1 = table2.column2 AND table1.column2 = '123'
                    selectWithWhere(fileCountFirstTable, fileCountSecondTable, dbManager, query, tablesFromQuery, columnsFromQuery);
                } else {
                    crossJoin(fileCountFirstTable, fileCountSecondTable, dbManager, tablesFromQuery.head->data, columnsFromQuery);
                }
                
            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "DELETE"){
            try {
                // DELETE FROM таблица1 WHERE таблица1.колонка1 = '123'
                // обрабатываем запрос
                    
                if (!(iss >> wordFromQuery) || wordFromQuery != "FROM") {
                    throw std::runtime_error("Incorrect command");
                }
                string tableName;
                if (!(iss >> tableName)) { // table1
                    throw runtime_error("Expected table name after 'FROM'");
                }
                
                if (!tableExists(dbManager, tableName)) {
                    throw std::runtime_error("Table does not exist");
                }
                if (isLocked(dbManager, tableName)){
                    throw std::runtime_error("Table is locked");
                }

                if (!(iss >> wordFromQuery) || wordFromQuery != "WHERE") {
                    throw runtime_error("Expected 'WHERE' clause");
                }

                if (!(iss >> wordFromQuery)) { // table1.column1 
                    throw runtime_error("Expected condition after 'WHERE'");
                }
                LinkedList tableFromQuery;
                LinkedList columnFromQuery;
                if (findDot(wordFromQuery) == string::npos){
                    throw runtime_error("Incorrect data format: expected table1.column1");
                }
                splitPoint(tableFromQuery, columnFromQuery, wordFromQuery);
                if (tableFromQuery.head->data != tableName){
                    throw runtime_error("Incorrect table in query");
                }

                if (!(iss >> wordFromQuery) || wordFromQuery != "=") { // =
                    throw std::runtime_error("Incorrect command");
                }

                locking(dbManager, tableName); // тут блокируем доступ к таблице

                string query;
                string valuesPart;
                getline(iss, valuesPart); // считываем оставшуюся часть строки (вдруг захотим удалять не по одному значению)
                if (valuesPart.empty()) {
                    throw runtime_error("Expected conditions");
                }
                query += valuesPart;
                deleteFunc(dbManager, tableName, query, tableFromQuery, columnFromQuery); // тут функция удаления

                unlocking(dbManager, tableName); // а тут разблокируем после произведения удаления

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "INSERT"){
            try {
                // обрабатываем запрос
                if (!(iss >> wordFromQuery) || wordFromQuery != "INTO") {
                    throw std::runtime_error("Incorrect command");
                }
                string tableName;
                if (!(iss >> tableName)) { // table1
                    throw runtime_error("Expected table name");
                }
                if (!tableExists(dbManager, tableName)) {
                    throw std::runtime_error("Table does not exist");
                }
                
                if (!(iss >> wordFromQuery) || wordFromQuery != "VALUES") {
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
                if (valuesPart.empty()) {
                    throw runtime_error("Expected conditions");
                }
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