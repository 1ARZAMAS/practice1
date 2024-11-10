#include "header.h"
#include "lock.h"
#include "functions.h"
#include "sqlFuncs.h"

using namespace std;
namespace fs = std::filesystem;

string getColumnValue(int& fileCountFirstTable, int& fileCountSecondTable, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, const string& columnName, int rowIndex1, int rowIndex2, const DatabaseManager& dbManager) {
    int pos_dot = columnName.find("."); // разделяем columnName таблицу и столбец "table1.column1" ->
    string tableName = columnName.substr(0, pos_dot); // table1
    string column = columnName.substr(pos_dot + 1); // column1

    int columnIdx;
    for (int i = 0; i < amountOfCSV(dbManager, tableName); i++) { // пройдемся по всем файлам таблицы
        string tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + std::to_string(i + 1) + ".csv";
        rapidcsv::Document doc(tableDir);// открываем CSV-файл с помощью rapidcsv
        columnIdx = doc.GetColumnIdx(column); // получаем индекс колонки
        if (columnIdx == -1) {
            cout << "Column wasn't found: " << column << endl;
            return "";
        }
        string cellValue;
        if (tableName == tablesFromQuery.tail->data) { // если первая таблица, то использует j для обхода (в цикле определено)
            cellValue = doc.GetCell<string>(columnIdx, rowIndex1);
            
        } else if (tableName == tablesFromQuery.head->data) { // если вторая таблица, то использует p для обхода (в цикле определено)
            cellValue = doc.GetCell<string>(columnIdx, rowIndex2);
        }
        return cellValue;
    }
    return "";
}

bool recursionFunc(int& fileCountFirstTable, int& fileCountSecondTable, const string& query, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, int rowIndex1, int rowIndex2, const DatabaseManager& dbManager){
    string cleanedQuery = cleanString(query);
    int pos_or = cleanedQuery.find("OR"); // ищем первое вхождение OR
    if (pos_or != string::npos) {
        string leftPart = cleanedQuery.substr(0, pos_or);  // отсекаем левую часть до OR
        string rightPart = cleanedQuery.substr(pos_or + 2);  // отсекаем правую часть после OR
        bool leftResult = recursionFunc(fileCountFirstTable, fileCountSecondTable, leftPart, tablesFromQuery, columnsFromQuery, rowIndex1, rowIndex2, dbManager);
        bool rightResult = recursionFunc(fileCountFirstTable, fileCountSecondTable, rightPart, tablesFromQuery, columnsFromQuery, rowIndex1, rowIndex2, dbManager);
       
        return leftResult || rightResult;  // если хотя бы одно истинно, возвращаем true
    }
    int pos_and = cleanedQuery.find("AND"); // ищем первое вхождение AND
    if (pos_and != string::npos) {
        string leftPart = cleanedQuery.substr(0, pos_and);  // отсекаем левую часть до AND
        string rightPart = cleanedQuery.substr(pos_and + 3);  // отсекаем правую часть после AND
        bool leftResult = recursionFunc(fileCountFirstTable, fileCountSecondTable, leftPart, tablesFromQuery, columnsFromQuery, rowIndex1, rowIndex2, dbManager);
        bool rightResult = recursionFunc(fileCountFirstTable, fileCountSecondTable, rightPart, tablesFromQuery, columnsFromQuery, rowIndex1, rowIndex2, dbManager);
        
        return leftResult && rightResult;  // если оба истинно, возвращаем true
    }
    int pos_equal = cleanedQuery.find('=');
    if (pos_equal != string::npos){
        string left = cleanString(cleanedQuery.substr(0, pos_equal));  // левая часть, например, table1.column1
        string right = cleanString(cleanedQuery.substr(pos_equal + 1));  // правая часть, например, 'value'
        
        string leftValue;
        string rightValue;

        if (!findDot(left)){ // если не нашли точку в выражении, значит, это просто строка для сравнения
            leftValue = cleanString(left); // почистим строку от лишних символов
        } else {
            leftValue = getColumnValue(fileCountFirstTable, fileCountSecondTable,tablesFromQuery, columnsFromQuery, left, rowIndex1, rowIndex2, dbManager);
        }

        if (!findDot(right)){ // если не нашли точку в выражении, значит, это просто строка для сравнения
            rightValue = cleanString(right); // почистим строку от лишних символов
        } else {
            rightValue = getColumnValue(fileCountFirstTable, fileCountSecondTable,tablesFromQuery, columnsFromQuery, right, rowIndex1, rowIndex2, dbManager);
        }
        return cleanString(leftValue) == cleanString(rightValue);
        
    }
    return false; // Если нет =, значит, условий нет
}

// < INSERT INTO table1 VALUES ('somedata', '123')
// < INSERT INTO table1 VALUES ('somedata1', '123')
// < INSERT INTO table2 VALUES ('somedthing', 'somedata')
// < INSERT INTO table2 VALUES ('somasd', 'soasd')
// < SELECT table1.column1, table2.column1 FROM table1, table2 WHERE table1.column1 = table2.column2 AND table1.column2 = '123'

void selectWithWhere(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& query, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery) {
    for (int i = 0; i < fileCountFirstTable; i++) { // Пройдемся по всем файлам первой таблицы
        DBtable& firstTable = reinterpret_cast<DBtable&>(dbManager.tables.head->data); // приводим к типу DBtable
        string currentTable1 = firstTable.tableName; // получаем имя таблицы
        string tableDir1 = dbManager.schemaName + "/" + currentTable1 + "/" + currentTable1 + "_" + std::to_string(i + 1) + ".csv";
        rapidcsv::Document document1(tableDir1); // открываем файл 1
        for (int j = 0; j < document1.GetRowCount(); j++) {
            for (int k = 0; k < fileCountSecondTable; k++) { // Проходимся по второй таблице
                DBtable& secondTable = reinterpret_cast<DBtable&>(dbManager.tables.head->next->data); // приводим к типу DBtable
                string currentTable2 = secondTable.tableName; // получаем имя таблицы
                string tableDir2 = dbManager.schemaName + "/" + currentTable2 + "/" + currentTable2 + "_" + std::to_string(k + 1) + ".csv";
                rapidcsv::Document document2(tableDir2); // открываем файл 2
                for (int p = 0; p < document2.GetRowCount(); p++) {
                    if (recursionFunc(fileCountFirstTable, fileCountSecondTable, query, tablesFromQuery, columnsFromQuery, j, p, dbManager)) {
                        //столбец, строка
                        for (int col = 0; col < document1.GetColumnCount(); col++) {
                            cout << document1.GetCell<string>(col, j) << " ";
                        }
                        cout << "| ";

                        // И всю строку из второй таблицы
                        for (int col = 0; col < document2.GetColumnCount(); col++) {
                            cout << document2.GetCell<string>(col, p) << "  ";
                        }
                        cout << endl;
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