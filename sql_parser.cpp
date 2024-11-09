#include "header.h"
#include "lock.h"
#include "functions.h"
#include "sqlFuncs.h"

using namespace std;
namespace fs = std::filesystem;

void crossJoin(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& tableName, LinkedList& columnsFromQuery){
    for (int i = 0; i < fileCountFirstTable; i++){ // пройдемся по всем файлам первой таблицы
        DBtable& firstTable = reinterpret_cast<DBtable&>(dbManager.tables.head->data); // приводим к типу DBtable
        string currentTable1 = firstTable.tableName; // получаем имя таблицы

        string tableDir1 = dbManager.schemaName + "/" + currentTable1 + "/" + currentTable1 + "_" + std::to_string(i + 1) + ".csv";
        string column1 = cleanString(columnsFromQuery.head->next->data);
        
        rapidcsv::Document document1(tableDir1); // открываем файл 1
        int indexFirstColumn = document1.GetColumnIdx(column1); // считываем индекс искомой колонки 1
        int amountRow1 = document1.GetRowCount(); // считываем количество строк в файле 1
        for (int j = 0; j < amountRow1; j++){ // проходимся по всем строкам
            for (int k = 0; k < fileCountSecondTable; k++){ // пройдемся по второй таблице
                DBtable& secondTable = reinterpret_cast<DBtable&>(dbManager.tables.head->next->data); // приводим к типу DBtable
                string currentTable2 = secondTable.tableName; // получаем имя таблицы
                
                string tableDir2 = dbManager.schemaName + "/" + currentTable2 + "/" + currentTable2 + "_" + std::to_string(k + 1) + ".csv";
                string column2 = cleanString(columnsFromQuery.head->data);
                rapidcsv::Document document2(tableDir2); // открываем файл 2

                int indexSecondColumn = document2.GetColumnIdx(column2); // считываем индекс искомой колонки 2
                int amountRow2 = document2.GetRowCount(); // считываем количество строк в файле 2
                for (int p = 0; p < amountRow2; ++p) {
                    cout << document1.GetCell<string>(0, j) << ": ";
                    cout << document1.GetCell<string>(indexFirstColumn, j) << "  |   ";
                    cout << document2.GetCell<string>(0, p) << ": ";
                    cout << document2.GetCell<string>(indexSecondColumn, p) << endl;
                }
            }
        }
    }
}

bool recursionFunc(string query){
    int pos_or = query.find("OR"); // ищем первое вхождение OR
    if (pos_or != string::npos) {
        string leftPart = query.substr(0, pos_or);  // отсекаем левую часть до OR
        string rightPart = query.substr(pos_or + 2);  // отсекаем левую часть после OR
        bool leftResult = recursionFunc(cleanString(leftPart));
        bool rightResult = recursionFunc(cleanString(rightPart));

        return leftResult || rightResult;  // если хотя бы одно истинно, вернем true
    }
    int pos_and = query.find("AND"); // ищем первое вхождение AND
    if (pos_and != string::npos) {
        string leftPart = query.substr(0, pos_and);  // отсекаем левую часть до AND
        string rightPart = query.substr(pos_and + 3);  // отсекаем левую часть после AND
        bool leftResult = recursionFunc(cleanString(leftPart));
        bool rightResult = recursionFunc(cleanString(rightPart));

        return leftResult && rightResult;  // если оба истинно, вернем true
    }
    int pos_equal = query.find('=');
    if (pos_equal != string::npos){
        string leftPart = cleanString(query.substr(0, pos_equal));  // левая часть до =
        string rightPart = cleanString(query.substr(pos_equal + 1));  // правая часть после =

    }
    return false; // если нет =, значит, условия точного нет и ничего выводить не будем
}

// SELECT table1.column1, table2.column1 FROM table1, table2 WHERE table1.column1 = table2.column2 AND table1.column2 = 'string'
// tablesFromQuery table1 table2
// columnsFromQuery column1 column1
void selectWithWhere(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& query, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery) {
    // if (!findDot(/**/)){ // Если не нашли точку в выражении, значит, что это просто строка, с которой будем сравнивать значение
    //     cleanString(/**/); // а для этого почистим строку от лишних символов
    // }

    for (int i = 0; i < fileCountFirstTable; i++){ // пройдемся по всем файлам первой таблицы
        DBtable& firstTable = reinterpret_cast<DBtable&>(dbManager.tables.head->data); // приводим к типу DBtable
        string currentTable1 = firstTable.tableName; // получаем имя таблицы
        string tableDir1 = dbManager.schemaName + "/" + currentTable1 + "/" + currentTable1 + "_" + std::to_string(i + 1) + ".csv";
        //string column1 = cleanString(columnsFromQuery.head->next->data);
        rapidcsv::Document document1(tableDir1); // открываем файл 1
        //int indexFirstColumn = document1.GetColumnIdx(column1); // считываем индекс искомой колонки 1
        for (int j = 0; j < document1.GetRowCount(); j++) {
            for (int k = 0; k < fileCountSecondTable; k++){ // пройдемся по второй таблице
                DBtable& secondTable = reinterpret_cast<DBtable&>(dbManager.tables.head->next->data); // приводим к типу DBtable
                string currentTable2 = secondTable.tableName; // получаем имя таблицы
                string tableDir2 = dbManager.schemaName + "/" + currentTable2 + "/" + currentTable2 + "_" + std::to_string(k + 1) + ".csv";
                //string column2 = cleanString(columnsFromQuery.head->data);
                rapidcsv::Document document2(tableDir2); // открываем файл 2
                //int indexSecondColumn = document2.GetColumnIdx(column2); // считываем индекс искомой колонки 2
                
                for (int p = 0; p < document2.GetRowCount(); ++p) {
                    // Применение условий фильтрации
                    if (recursionFunc(query, tablesFromQuery, columnsFromQuery)) {
                        // Если условие выполнено, выводим соответствующие данные
                        for (int p = 0; p < document2.GetRowCount(); ++p) {
                            // cout << document1.GetCell<string>(0, j) << ": ";
                            // cout << document1.GetCell<string>(indexFirstColumn, j) << "  |   ";
                            // cout << document2.GetCell<string>(0, p) << ": ";
                            // cout << document2.GetCell<string>(indexSecondColumn, p) << endl;
                        }
                    }
                }
            }
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

                iss >> wordFromQuery;
                if (wordFromQuery != "FROM") {
                    throw std::runtime_error("Incorrect command");
                }
                // проверка на то, что названия таблиц из table1.column1 будут такими же как и после FROM, те table1
                // (условно)
                string tableName;
                iss >> tableName;
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
                iss >> tableName;
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
                if (nextWord == "WHERE"){ // проверим, есть ли следующее слово WHERE
                    hasWhere = true;
                }

                if (hasWhere) {
                    string query;
                    string valuesPart;
                    getline(iss, valuesPart); // считываем оставшуюся часть строки
                    query += valuesPart; // table1.column1 = table2.column2 AND table1.column2 = '123'
                    selectWithWhere(fileCountFirstTable, fileCountSecondTable, dbManager, query);
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