#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include "header.h"

using namespace std;

void QueryManager(const DatabaseManager& dbManager) {
    cout << "< ";
    std::string SQLquery;
    getline(cin, SQLquery);
    LinkedList wordsFromQuery;
    std::string space = " ";
    size_t start = 0;
    size_t end = SQLquery.find(space);
    
    // разделение строки на слова и добавление в связный список
    while (end != std::string::npos) {
        wordsFromQuery.addToTheEnd(SQLquery.substr(start, end - start));
        start = end + space.length();
        end = SQLquery.find(space, start);
    }
    wordsFromQuery.addToTheEnd(SQLquery.substr(start, end));

    // теперь wordsFromQuery содержит все слова из SQL-запроса
    // проверка первого слова
    if (wordsFromQuery.head->data == "SELECT" && wordsFromQuery.head->next->data == "FROM") {
        try {
            cout << "SELECT WORKED!" << endl;
            //ParseSelect(wordsFromQuery, fileDirectory, schemaName, JSONSchema);
        } catch (const std::exception& ErrorInfo) {
            std::cerr << ErrorInfo.what() << std::endl;
        }
    } else if (wordsFromQuery.head->data == "INSERT" && wordsFromQuery.head->next->data == "INTO") {
        try {
            cout << "INSERT WORKED!" << endl;
            //ParsingInsert(wordsFromQuery, fileDirectory, schemaName, JSONSchema);
        } catch (const std::exception& ErrorInfo) {
            std::cerr << ErrorInfo.what() << std::endl;
        }
    } else if (wordsFromQuery.head->data == "DELETE" && wordsFromQuery.head->next->data == "FROM") {
        try {
            cout << "DELETE WORKED!" << endl;
            //ParsingDelete(wordsFromQuery, fileDirectory, schemaName, JSONSchema);
        } catch (const std::exception& ErrorInfo) {
            std::cerr << ErrorInfo.what() << std::endl;
        }
    }
}
