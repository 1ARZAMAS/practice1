#include <iostream>
#include <fstream>
#include <string>
#include "header.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

int main() {
    system("chcp 65001");
    loadSchema("schema.json"); // Парсим содержимое json файла
    DatabaseManager dbManager;
    try {
        loadSchema(dbManager, "schema.json");
        createDirectoriesAndFiles(dbManager);
        std::cout << "Файлы успешно были созданы." << std::endl;
        QueryManager(dbManager);

    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }
    return 0;
}