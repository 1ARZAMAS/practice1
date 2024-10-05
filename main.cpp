#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "json.hpp" // Библиотека для работы с JSON

void writeToFile(const std::string& filename, const std::string& text) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << text;
        file.close();
    } else {
        throw std::logic_error("Невозможно записать данные в файл: " + filename);
    }
}

std::string readFromFile(const std::string& filename) {
    std::ifstream file(filename);
    std::string content;
    if (file.is_open()) {
        std::string line;
        while (getline(file, line)) {
            content += line + "\n";
        }
        file.close();
        return content;
    } else {
        throw std::logic_error("Невозможно прочитать файл: " + filename);
    }
}


void createDirectory(const std::string& dirPath) {
    std::filesystem::create_directories(dirPath);
}

using json = nlohmann::json;
namespace fs = std::filesystem;

class SQLParser {
public:
    
    void parseSelect(const std::string& query) {
        std::string tableName;
        std::vector<std::string> columns;

        std::string select = "SELECT";
        std::string from = "FROM";
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        size_t selectPos = lowerQuery.find(select);
        size_t fromPos = lowerQuery.find(from);
        if (selectPos == std::string::npos || fromPos == std::string::npos) {
            throw std::invalid_argument("Invalid SELECT query.");
        }

        std::string columnsPart = query.substr(selectPos + select.length(), fromPos - selectPos - select.length());
        std::istringstream colStream(columnsPart);
        std::string column;
        while (std::getline(colStream, column, ',')) {
            columns.push_back(trim(column));
        }

        tableName = query.substr(fromPos + from.length());
        tableName = trim(tableName);

        std::cout << "Parsed SELECT Query:\n";
        std::cout << "Columns: ";
        for (const auto& col : columns) {
            std::cout << col << " ";
        }
        std::cout << "\nTable: " << tableName << std::endl;
    }

    void parseInsert(const std::string& query) {
        std::string tableName;
        std::vector<std::string> values;

        std::string insert = "INSERT INTO";
        std::string valuesStr = "VALUES";
        std::string lowerQuery = query;
        std::transform(lowerQuery.begin(), lowerQuery.end(), lowerQuery.begin(), ::tolower);

        size_t insertPos = lowerQuery.find(insert);
        size_t valuesPos = lowerQuery.find(valuesStr);
        if (insertPos == std::string::npos || valuesPos == std::string::npos) {
            throw std::invalid_argument("Invalid INSERT query.");
        }

        std::string tablePart = query.substr(insertPos + insert.length(), valuesPos - insertPos - insert.length());
        tableName = trim(tablePart);

        std::string valuesPart = query.substr(valuesPos + valuesStr.length());
        std::istringstream valStream(valuesPart);
        std::string value;
        while (std::getline(valStream, value, ',')) {
            values.push_back(trim(value));
        }

        std::cout << "Parsed INSERT Query:\n";
        std::cout << "Table: " << tableName << "\nValues: ";
        for (const auto& val : values) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        // Вставка данных в CSV
        insertIntoTable(tableName, values);
    }

    void insertIntoTable(const std::string& tableName, const std::vector<std::string>& values) {
        std::cout << "Inserting into " << tableName << ": ";
        for (const auto& val : values) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
        // Логика для записи в CSV...
    }

private:
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(' ');
        size_t last = str.find_last_not_of(' ');
        return (first == std::string::npos) ? "" : str.substr(first, (last - first + 1));
    }
};

class Database {
public:
    Database(const std::string& configPath) {
        loadSchema(configPath);
        createDirectoriesAndFiles();
    }

private:
    std::string schemaName;
    std::vector<std::string> tables;
    int tuplesLimit;

    void loadSchema(const std::string& configPath) {
        std::ifstream file(configPath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open schema file.");
        }

        json schema;
        file >> schema;
        schemaName = schema["name"];
        tuplesLimit = schema["tuples_limit"];

        for (const auto& table : schema["structure"].items()) {
            tables.push_back(table.key());
        }
    }

    void createDirectoriesAndFiles() {
        if (!fs::exists(schemaName)) {
            fs::create_directory(schemaName);
        }

        for (const auto& table : tables) {
            std::string tableDir = schemaName + "/" + table;
            if (!fs::exists(tableDir)) {
                fs::create_directory(tableDir);
            }

            createCSVFile(tableDir, table);
            createPKSequenceFile(tableDir, table);
            createLockFile(tableDir, table);
        }
    }

    void createCSVFile(const std::string& tableDir, const std::string& tableName) {
        std::string csvFilePath = tableDir + "/1.csv";
        std::cout << "Attempting to create CSV file at: " << csvFilePath << std::endl; // Вывод пути
        std::ofstream csvFile(csvFilePath);
        if (!csvFile.is_open()) {
            throw std::runtime_error("Could not create CSV file at " + csvFilePath);
        }

        csvFile << tableName << "_pk,"; // Первая колонка для первичного ключа
        csvFile << "column1,column2\n"; // Пример заголовков
        csvFile.close();
    }


    void createPKSequenceFile(const std::string& tableDir, const std::string& tableName) {
        std::string pkFilePath = tableDir + "/" + tableName + "_pk_sequence.txt";
        std::ofstream pkFile(pkFilePath);
        if (!pkFile.is_open()) {
            throw std::runtime_error("Could not create primary key sequence file.");
        }

        pkFile << "0\n"; // Начинаем с 0
        pkFile.close();
    }

    void createLockFile(const std::string& tableDir, const std::string& tableName) {
        std::string lockFilePath = tableDir + "/" + tableName + "_lock.txt";
        std::ofstream lockFile(lockFilePath);
        if (!lockFile.is_open()) {
            throw std::runtime_error("Could not create lock file.");
        }

        lockFile << "unlocked\n"; // Начальное состояние - разблокировано
        lockFile.close();
    }
};

int main() {
    system("chcp 65001");
    std::string path = "C:/GIT/practice1/Схема_1/таблица1/";
    
    // Создаем директорию, если она не существует
    if (!std::filesystem::exists(path)) {
        if (!std::filesystem::create_directories(path)) {
            std::cerr << "Не удалось создать директорию: " << path << std::endl;
            return 1;
        }
    }

    std::string filename = path + "test.txt";
    
    try {
        // Запись в файл
        writeToFile(filename, "Test file created.");
        std::cout << "Файл успешно создан: " << filename << std::endl;

        // Чтение из файла
        std::string content = readFromFile(filename);
        std::cout << "Содержимое файла:\n" << content << std::endl;
    } catch (const std::logic_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}