#include <string>
#include <iostream>
#include <fstream>
#include <ostream>
#include "header.h"

void loadSchema(DatabaseManager& dbManager, const std::string& filePath, HashTable& schemaHashTable) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Не удалось открыть файл: " << filePath << std::endl;
        return;
    }

    json schema;
    file >> schema; // Парсим JSON

    // Извлечение данных
    std::string name = schema["name"];
    int tuples_limit = schema["tuples_limit"];

    // Извлечение структуры
    for (auto& [tableName, columns] : schema["structure"].items()) {
        std::string columnList;
        for (const auto& column : columns) {
            columnList += column.get<std::string>() + ",";
        }
        // Удаляем последний запятую
        if (!columnList.empty()) {
            columnList.pop_back();
        }
        // Добавляем в хэш-таблицу
        schemaHashTable.push(tableName, columnList);
    }

}

