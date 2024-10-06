#include <fstream>
#include <string>
#include <sys/stat.h> //для mkdir
#include <iostream>
#include <stdexcept> //для исключений
#include <thread> //для потоков
#include <chrono>
#include <ctime> //для времени

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

void lock_table(const std::string& table_name, const std::string& lock_file_path, int timeout_seconds) { // нельзя использовать, если уже кто-то использует json
    std::ofstream lock_file(lock_file_path);
    if (!lock_file.is_open()) {
        throw std::runtime_error("Failed to open lock file: " + lock_file_path);
    }

    std::time_t start_time = std::time(nullptr);
    while (lock_file.good()) {
        if (std::time(nullptr) - start_time > timeout_seconds) {
            throw std::runtime_error("Timeout waiting for table lock: " + table_name);
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}