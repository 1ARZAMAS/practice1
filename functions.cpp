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
    // убираем пробелы
    int start = cleaned.find_first_not_of(" \t");
    int end = cleaned.find_last_not_of(" \t");
    if (start == string::npos || end == string::npos) {
        return ""; // если только пробелы, вернем пустую строку
    }
    return cleaned.substr(start, end - start + 1);
}

bool findDot(string str){
    if (str.find('.')){
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
