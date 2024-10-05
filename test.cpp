#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

class Database {
public:
    Database(const std::string& configPath) {
        loadSchema(configPath);
        createDirectoriesAndFiles();
    }

    void executeQuery(const std::string& query) {
        if (query.find("SELECT") != std::string::npos) {
            parseSelect(query);
        } else if (query.find("INSERT INTO") != std::string::npos) {
            parseInsert(query);
        } else if (query.find("DELETE FROM") != std::string::npos) {
            parseDelete(query);
        } else {
            throw std::invalid_argument("Unsupported query.");
        }
    }

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
        }
    }

    void createCSVFile(const std::string& tableDir, const std::string& tableName) {
        std::cout << "Attempting to create CSV file at: " << fs::absolute(tableDir) << std::endl;

        fs::path csvPath = fs::absolute(tableDir); // Convert to path
        std::ofstream csvFile(csvPath/"gaga.txt");
        if (!csvFile.is_open()) {
            std::cerr << "Could not create CSV file at " << fs::absolute(tableDir) << " (error: " << strerror(errno) << ")" << std::endl;
        }

        csvFile << tableName << "_pk,"; // Первая колонка для первичного ключа
        csvFile << "column1,column2\n"; // Пример заголовков
        csvFile.close();
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

        insertIntoTable(tableName, values);
    }

    void insertIntoTable(const std::string& tableName, const std::vector<std::string>& values) {
        std::cout << "Inserting into " << tableName << ": ";
        for (const auto& val : values) {
            std::cout << val << " ";
        }
        std::cout << std::endl;

        // Логика для записи в CSV
        std::string tableDir = schemaName + "/" + tableName;
        std::string csvFilePath = tableDir + "/1.csv";
        
        std::ofstream csvFile(csvFilePath, std::ios::app);
        if (!csvFile.is_open()) {
            throw std::runtime_error("Could not open CSV file for appending: " + csvFilePath);
        }

        std::string line = createCsvLine(values);
        csvFile << line << "\n";
        csvFile.close();
    }

    std::string createCsvLine(const std::vector<std::string>& values) {
        std::string line;
        for (const auto& val : values) {
            line += val + ",";
        }
        if (!line.empty()) {
            line.pop_back(); // Убираем последнюю запятую
        }
        return line;
    }
};

int main() {
    system("chcp 65001");
    try {
        Database db("schema.json");

        std::string query;
        std::cout << "Введите SQL-запрос (или 'exit' для выхода):" << std::endl;

        while (true) {
            std::getline(std::cin, query); // Чтение запроса из консоли

            if (query == "exit") {
                break; // Выход из цикла
            }

            // Выполнение запроса
            try {
                db.executeQuery(query);
            } catch (const std::exception& e) {
                std::cerr << "Ошибка выполнения запроса: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }

    return 0;
}

