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
    DBtable table;
    try {
        loadSchema(dbManager, "schema.json");
        createDirectoriesAndFiles(dbManager);
        std::cout << "Files were successfuly created" << std::endl;
        QueryManager(dbManager, table);

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return 0;
}