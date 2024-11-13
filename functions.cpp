#include "header.h"
using namespace std;

bool tableExists(const DatabaseManager& dbManager, const string& tableName) {
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

void splitPoint(LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, string& wordFromQuery) {
    size_t dotPos = wordFromQuery.find('.'); // найдем позицию точки
    if (dotPos != string::npos) {
        tablesFromQuery.addToTheHead(wordFromQuery.substr(0, dotPos)); // Сохраняем имя таблицы
        columnsFromQuery.addToTheHead(wordFromQuery.substr(dotPos + 1)); // Сохраняем имя колонки
    } else {
        cout << "Incorrect format: " << wordFromQuery << endl;
        return;
    }
}

string cleanString(const string& str) {
    string cleaned = str;
    if (!cleaned.empty() && cleaned.back() == ',') {
        cleaned.pop_back(); // убираем последнюю запятую
    }
    
    if (!cleaned.empty() && (cleaned.front() == '\'' || cleaned.front() == '\"') &&
        (cleaned.back() == '\'' || cleaned.back() == '\"')) {
        cleaned = cleaned.substr(1, cleaned.size() - 2);  // Убираем первую и последнюю кавычку
    }
    // убираем пробелы
    int start = cleaned.find_first_not_of(" \t");
    int end = cleaned.find_last_not_of(" \t");
    if (start == string::npos || end == string::npos) {
        return ""; // если только пробелы, вернем пустую строку
    }
    return cleaned.substr(start, end - start + 1);
}

bool findDot(string str){
    if (str.find('.') != -1){
        return true;
    } else {
        return false;
    }
}

int amountOfCSV(const DatabaseManager& dbManager, const string& tableName) {
    int amount = 0; // ищем количество созданных csv файлов
    string tableDir;
    while (true) {
        tableDir = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + to_string(amount + 1) + ".csv";
        
        ifstream file(tableDir);
        if (!file.is_open()) { // если файл нельзя открыть, то он не существует
            break;
        }
        amount++;
        file.close();
    }
    return amount; // возвращаем количество найденных файлов
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