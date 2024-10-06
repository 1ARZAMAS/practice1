#include <iostream>
#include <fstream>
#include <string>
#include "header.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;


int main() {
    loadSchema("schema.json"); // Парсим содержимое json файла
    DatabaseManager dbManager;
    try {
        loadSchema(dbManager, "schema.json");
        createDirectoriesAndFiles(dbManager);
        std::cout << "Schema and files created successfully." << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}