#include <iostream>
#include <fstream>
#include <string>
#include "header.h"
#include "json.hpp"

using json = nlohmann::json;

using namespace std;

int main() {
    DatabaseManager dbManager;
    DBtable dbTableManager;
    try {
        loadSchema(dbManager, "schema.json");
        createDirectoriesAndFiles(dbManager);
        std::cout << "Files were successfuly created" << std::endl;
        QueryManager(dbManager);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}