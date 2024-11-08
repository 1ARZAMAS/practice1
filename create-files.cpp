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

    // для каждой таблицы создаем новый временный объект DBtable
    for (const auto& table : schema["structure"].items()) {
        DBtable tempTable;
        tempTable.tableName = table.key();
        tempTable.columnName.clear(); // очищаем перед добавлением новых колонок

        // добавляем все колонки текущей таблицы в список
        for (const std::string& column : table.value()) {
            tempTable.columnName.addToTheEnd(column);
        }
        
        dbManager.tables.addToTheEndUni(tempTable); // добавляем таблицу в менеджер
    }
}

void createDirectoriesAndFiles(const DatabaseManager& dbManager) {
    if (!fs::exists(dbManager.schemaName)) { // если директории нет, то мы ее создадаем
        fs::create_directory(dbManager.schemaName); 
    }
    
    UniversalNode* current = dbManager.tables.head;
    while (current != nullptr) {
        std::string tableDir = dbManager.schemaName + "/" + current->data.tableName;
        if (!fs::exists(tableDir)) {
            fs::create_directory(tableDir);
        }
        createCSVFile(tableDir, current->data, dbManager.tuplesLimit); // создаем CSV файлы

        createPrimaryKeyFile(tableDir, current->data.tableName); // создадим файл с ключом (1)

        createLockFile(tableDir, current->data.tableName); // создадим файл состояния
        
        current = current->next; // и так сделаем для каждого значения таблицы
    }
}

void createCSVFile(const std::string& tableDir, DBtable& table, int tuplesLimit) {
    int fileIndex = 1;
    fs::path csvPath = fs::path(tableDir) / (table.tableName + "_" + std::to_string(fileIndex) + ".csv");
    
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Error while making CSV file in " << fs::absolute(csvPath) << std::endl;
        return;
    }

    csvFile << table.tableName << "_pk"; // Записываем первичный ключ

    // Запись колонок
    Node* currentColumn = table.columnName.head; // Начинаем с головы списка 
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
        std::cerr << "Error while making primary key for " << tableName << std::endl;
        return;
    }

    pkFile << 1; // Начальное значение первичного ключа
    pkFile.close();
}

void createLockFile(const std::string& tableDir, const std::string& tableName) {
    std::ofstream lockFile(fs::path(tableDir) / (tableName + "_lock.txt"));
    if (!lockFile.is_open()) {
        std::cerr << "Error while making lock file for " << tableName << std::endl;
        return;
    }

    lockFile << "unlocked"; // Статус блокировки
    lockFile.close();
}