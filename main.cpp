#include <iostream>
#include <fstream>
#include <string>
#include "header.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

int main() {
    system("chcp 65001");
    
    DatabaseManager dbManager; // Создание экземпляра DatabaseManager
    HashTable schemaHashTable; // Создание экземпляра HashTable

    try {
        // Передаем dbManager и schemaHashTable в функцию loadSchema
        cout << "test1"<< endl;
        std::string filePath = fs::absolute("schema.json").string();
        std::cout << "Путь к файлу schema.json: " << filePath << std::endl;

        loadSchema(dbManager, filePath, schemaHashTable); 
        cout << "test2"<< endl;
        createDirectoriesAndFiles(dbManager, schemaHashTable);
        cout << "test3"<< endl;
        std::cout << "Файлы успешно были созданы." << std::endl;

        QueryManager(dbManager);

    } catch (const std::exception& ex) {
        std::cerr << "Ошибка: " << ex.what() << std::endl;
    }
    return 0;
}
