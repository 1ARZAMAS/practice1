#include <fstream>
#include <string>
#include <sys/stat.h> //для mkdir
#include <iostream>
#include <stdexcept> //для исключений
#include <thread> //для потоков
#include <chrono>
#include <ctime> //для времени

void CreateDir(const std::string& dirName) {
    int status = mkdir(dirName.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status != 0) {
        std::cerr << "Error creating directory: " << dirName << std::endl;
    }
}

void CreateFile(const std::string& path, const std::string& filename, const std::string& content, bool append) {
    std::string fullPath = path + "/" + filename;
    std::ifstream file(fullPath);
    if (file.good()) {
        // Файл уже существует
        if (append) {
            // Добавляем содержимое к существующему файлу
            std::ofstream outFile(fullPath, std::ios_base::app);
            outFile << content;
            outFile.close();
        } else {
            // Перезаписываем существующий файл
            std::ofstream outFile(fullPath, std::ios_base::trunc);
            outFile << content;
            outFile.close();
        }
    } else {
        // Файл не существует, создаем его
        std::ofstream outFile(fullPath);
        outFile << content;
        outFile.close();
    }
}

void lock_table(const std::string& table_name, const std::string& lock_file_path, int timeout_seconds) {
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