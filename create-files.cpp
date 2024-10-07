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

void loadSchema(DatabaseManager& dbManager, const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка открытия schema.json файла");
    }

    json schema;
    file >> schema;
    dbManager.schemaName = schema["name"];
    dbManager.tuplesLimit = schema["tuples_limit"];

    for (const auto& table : schema["structure"].items()) {
        const std::string tableName = table.key();
        const auto& columns = table.value();
        LinkedList columnList; // Создаем новый список для колонок
        
        for (const auto& column : columns) {
            dbManager.hashTable.push(tableName, column.get<std::string>()); 
            //columnList.addToTheEnd(column.get<std::string>()); // Добавляем колонки в список
        }

        // Здесь добавляем таблицу и её колонки в хеш-таблицу
        
    }
}

void createDirectoriesAndFiles(const DatabaseManager& dbManager) {
    if (!fs::exists(dbManager.schemaName)) {
        fs::create_directory(dbManager.schemaName);
    }

    Node* current = dbManager.tables.head;
    while (current != nullptr) {
        std::string table = current->data;
        std::string tableDir = dbManager.schemaName + "/" + table;

        if (!fs::exists(tableDir)) {
            fs::create_directory(tableDir);
        }

        createCSVFile(tableDir, table, dbManager);
        createPrimaryKeyFile(tableDir, table);
        createLockFile(tableDir, table);
        
        current = current->next;
    }
}

void createCSVFile(const std::string& tableDir, const std::string& tableName, const DatabaseManager& dbManager) {
    int fileIndex = 1;
    fs::path csvPath = fs::path(tableDir) / (tableName + "_" + std::to_string(fileIndex) + ".csv");
    
    // Создание начального CSV файла
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Ошибка создания CSV файла в " << fs::absolute(csvPath) << std::endl;
        return;
    }

    // Получение колонок из хеш-таблицы
    HashTableItem* item = dbManager.hashTable.get(tableName); // Получаем элемент таблицы по имени
    if (item == nullptr) {
        std::cerr << "Таблица " << tableName << " не найдена." << std::endl;
        csvFile.close();
        return;
    }

    // Запись заголовков
    csvFile << tableName << "_pk"; // Записываем первичный ключ

    // Запись колонок
    LinkedList& columns = item->columns; // Получаем список колонок
    Node* currentColumn = columns.head; // Начинаем с головы списка
    //dbManager.hashTable.get(tableName, column.get<std::string>()); 

    while (currentColumn != nullptr) {
        csvFile << "," << currentColumn->data; // Записываем имя колонки
        currentColumn = currentColumn->next; // Переходим к следующему элементу
    }

    csvFile << "\n"; // Переход на новую строку
    csvFile.close(); // Закрываем файл
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