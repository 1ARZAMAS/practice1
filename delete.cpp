#include "header.h"
#include "functions.h"
using namespace std;

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
