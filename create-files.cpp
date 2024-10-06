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

struct DatabaseManager {
    std::string schemaName;
    int tuplesLimit;
    LinkedList tables;
};

void loadSchema(DatabaseManager& dbManager, const std::string& configPath) {
    std::ifstream file(configPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open schema file.");
    }

    json schema;
    file >> schema;
    dbManager.schemaName = schema["name"];
    dbManager.tuplesLimit = schema["tuples_limit"];

    for (const auto& table : schema["structure"].items()) {
        dbManager.tables.addToTheEnd(table.key());
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

        createCSVFile(tableDir, table);
        createPrimaryKeyFile(tableDir, table);
        createLockFile(tableDir, table);
        
        current = current->next;
    }
}

void createCSVFile(const std::string& tableDir, const std::string& tableName) {
    int fileIndex = 1;
    fs::path csvPath = fs::path(tableDir) / (tableName + "_" + std::to_string(fileIndex) + ".csv");
    
    // Создание начального CSV файла
    std::ofstream csvFile(csvPath);
    if (!csvFile.is_open()) {
        std::cerr << "Could not create CSV file at " << fs::absolute(csvPath) << std::endl;
        return;
    }

    // Запись заголовков
    csvFile << tableName << "_pk,column1,column2,column3,column4\n";
    csvFile.close();
}

void createPrimaryKeyFile(const std::string& tableDir, const std::string& tableName) {
    std::ofstream pkFile(fs::path(tableDir) / (tableName + "_pk_sequence.txt"));
    if (!pkFile.is_open()) {
        std::cerr << "Could not create primary key file for " << tableName << std::endl;
        return;
    }

    pkFile << 1; // Начальное значение первичного ключа
    pkFile.close();
}

void createLockFile(const std::string& tableDir, const std::string& tableName) {
    std::ofstream lockFile(fs::path(tableDir) / (tableName + "_lock.txt"));
    if (!lockFile.is_open()) {
        std::cerr << "Could not create lock file for " << tableName << std::endl;
        return;
    }

    lockFile << "unlocked"; // Статус блокировки
    lockFile.close();
}