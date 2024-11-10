#include "header.h"
#include "functions.h"
using namespace std;

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