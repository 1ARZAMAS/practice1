#include <iostream>
#include <string>
#include <fstream>
#include <stdexcept>
#include <ostream>
#include <filesystem>
#include "json.hpp"
#include "header.h"

namespace fs = std::filesystem;
using json = nlohmann::json;

std::string join(const json& columns) {
    std::string result;
    for (size_t i = 0; i < columns.size(); ++i) {
        result += columns[i].get<std::string>();
        if (i < columns.size() - 1) {
            result += ",";
        }
    }
    return result;
}

void loadSchema(DatabaseManager& dbManager, const std::string& filePath, const HashTable& schemaHashTable) {
    
    std::cout << "Проверка наличия файла: " << filePath << std::endl;
    if (!fs::exists(filePath)) {
        throw std::runtime_error("Файл schema.json не найден");
    }

    std::cout << "test4" << std::endl;
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка открытия schema.json файла");
    }

    std::cout << "test5" << std::endl;
    json schema;
    file >> schema;
    dbManager.schemaName = schema["name"];
    dbManager.tuplesLimit = schema["tuples_limit"];

    for (const auto& table : schema["structure"].items()) {
        dbManager.tables.addToTheEnd(table.key());
        std::string columns = join(table.value());

        //schemaHashTable.push(table.key(), columns); // Сохраняем колонки в хеш-таблицу
    }
}

void createCSVFile(const std::string& tableDir, const std::string& tableName, const HashTable& schemaHashTable) {
    std::cout << "test6" << std::endl;
    int fileIndex = 1;
    fs::path csvPath = fs::path(tableDir) / (tableName + "_" + std::to_string(fileIndex) + ".csv");

    std::string columns = schemaHashTable.get(tableName);
    std::cout << "Полученные колонки для " << tableName << ": " << columns << std::endl;

    // Проверка на пустые колонки
    if (columns.empty()) {
        std::cerr << "Не удалось получить колонки для " << tableName << std::endl;
        return; // Выход из функции, если колонки не найдены
    }
    std::cout << "test7" << std::endl;
    // Создание начального CSV файла
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Ошибка создания CSV файла в " << fs::absolute(csvPath) << std::endl;
        return;
    }
    // if (columns.empty()) {
    //     std::cerr << "Таблица " << tableName << " не найдена в схеме." << std::endl;
    //     return;
    // }
    std::cout << "test8" << std::endl;
    // Запись заголовков
    csvFile << tableName << "_pk," << columns << "\n";
    std::cout << "test9" << std::endl;
    csvFile.close();
}

void createPrimaryKeyFile(const std::string& tableDir, const std::string& tableName) {
    std::ofstream pkFile(fs::path(tableDir) / (tableName + "_pk_sequence.txt"));
    if (!pkFile.is_open()) {
        std::cerr << "Невозможно создать первичный ключ для " << tableName << std::endl;
        return;
    }

    pkFile << 1; // Начальное значение первичного ключа
    pkFile.close();
}

void createLockFile(const std::string& tableDir, const std::string& tableName) {
    std::ofstream lockFile(fs::path(tableDir) / (tableName + "_lock.txt"));
    if (!lockFile.is_open()) {
        std::cerr << "Невозможно создать замок для " << tableName << std::endl;
        return;
    }

    lockFile << "Не заблокировано"; // Статус блокировки
    lockFile.close();
}

void createDirectoriesAndFiles(const DatabaseManager& dbManager, const HashTable& schemaHashTable) {
    //if (!fs::exists(dbManager.schemaName)) {
        fs::create_directory(dbManager.schemaName);
    //}

    Node* current = dbManager.tables.head;
    while (current != nullptr) {
        std::string table = current->data;
        std::string tableDir = dbManager.schemaName + "/" + table;

        if (!fs::exists(tableDir)) {
            fs::create_directory(tableDir);
        }

        createCSVFile(tableDir, table, schemaHashTable);
        createPrimaryKeyFile(tableDir, table);
        createLockFile(tableDir, table);
        
        current = current->next;
    }
}