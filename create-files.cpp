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

// void tableClear(DBtable& table){
    //table.tableName = "";
    //table.columnName.clear();
    //table.tableValues.
    //int tuples = 0;
//}

void loadSchema(DatabaseManager& dbManager, const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Ошибка открытия schema.json файла");
    }

    json schema;
    file >> schema;
    dbManager.schemaName = schema["name"];
    dbManager.tuplesLimit = schema["tuples_limit"];
    DBtable tempTable;
    for (const auto& table : schema["structure"].items()) {     
        tempTable.tableName = table.key();
        for (const std::string column :  table.value()) {
            tempTable.columnName.addToTheEnd(column); // Добавляем колонки в список
        }
        dbManager.tables.addToTheEndUni(tempTable);
        // tempTable.tableName = "hjhjhjjhjhjhjhjh";
        // tempTable.columnName.clear();
        // tempTable.tuples = 0;
    }
}

void createDirectoriesAndFiles(const DatabaseManager& dbManager) {
    if (!fs::exists(dbManager.schemaName)) {
        fs::create_directory(dbManager.schemaName);
    }
    
    UniversalNode* current = dbManager.tables.head;
    while (current != nullptr) {
        std::string tableDir = dbManager.schemaName + "/" + current->data.tableName;
        if (!fs::exists(tableDir)) {
            fs::create_directory(tableDir);
        }
        createCSVFile(tableDir, current->data, dbManager.tuplesLimit);

        createPrimaryKeyFile(tableDir, current->data.tableName);

        createLockFile(tableDir, current->data.tableName);
        
        current = current->next;
    }
}

void createCSVFile(const std::string& tableDir, DBtable& table, int tuplesLimit) {
    int fileIndex = 1;
    fs::path csvPath = fs::path(tableDir) / (table.tableName + "_" + std::to_string(fileIndex) + ".csv");
    
    // Создание начального CSV файла
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Ошибка создания CSV файла в " << fs::absolute(csvPath) << std::endl;
        return;
    }

    // Запись заголовков
    csvFile << table.tableName << "_pk"; // Записываем первичный ключ

    // Запись колонок
    Node* currentColumn = table.columnName.head; // Начинаем с головы списка
    //table..get(tableName); 

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