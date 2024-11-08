#include "header.h"
using namespace std;

bool isLocked(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt";
        
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
        return true; // так как прочитать файл не смогли, вернем, что таблица заблокирована к редактированию
    }
    string current; // чтение текущего значения блокировки
    file >> current;
    file.close();
    if (current == "locked") {
        return true; // заблокирована
    }
    return false; // разблокирована
}

void locking(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt"; 
    // блокируем через обновление txt файла
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
        return;
    }
    file << "locked";
    file.close();
}

void unlocking(const DatabaseManager& dbManager, const std::string& tableName){
    string fileName = dbManager.schemaName + "/" + tableName + "/" + tableName + "_" + "lock.txt";
    // а тут разблокируем, тоже с помощью файла
    ofstream file(fileName);
    if (!file.is_open()) {
        cerr << "Error while reading lock file" << endl;
        return;
    }
    file << "unlocked";
    file.close();
}
