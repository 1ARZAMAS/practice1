#include "header.h"
#include "functions.h"
using namespace std;

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
    int counter = 1; // Все значения, которые захотим записать
    while (current != nullptr){// начнем с 1, потому что первая колонка - это ключ
        counter++;
        current = current->next;
    }
    
    string tempString;
    csv >> tempString;
    int tempCounter = 1; // количество колонок
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
    if (tempCounter == counter){
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
    }
    
    if (tempCounter > counter){ // если количество записываемых значений меньше, чем колонок, то будем записывать NULL
        Node* currentData = dataList.head; // пройдемся по всем значениям
        while(currentData != nullptr){ // и запишем их
            csv1 << currentData->data;
            csv1 << ","; // значит, не конец списка, ставим запятую
            currentData = currentData->next;
        }
        int diff = tempCounter - counter;
        for(; tempCounter > counter; counter++){
            csv1 << "NULL";
            if (tempCounter > counter + 1){
                csv1 << ",";
            } else {
                csv1 << "\n";
            }
        }
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
