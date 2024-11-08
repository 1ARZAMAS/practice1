#include <string>
#include <iostream>
#include <fstream>
#include <cstddef>
#include <thread>
#include <sstream>
#include "json.hpp"
#include "rapidcsv.hpp"

namespace fs = std::filesystem;
using json = nlohmann::json;

struct Node {
    std::string data;
    Node* next;
    Node* prev;

    Node(const std::string& value, Node* nextNode = nullptr, Node* prevNode = nullptr)
        : data(value), next(nextNode), prev(prevNode) {
    }
};

struct LinkedList{ 
    Node* head;
    Node* tail;

    LinkedList() : head(nullptr), tail(nullptr) {}

    void addToTheHead(std::string value); // добавление элемента в голову
    void addToTheEnd(std::string value); // добавление элемента в хвост
    void removeFromTheHead(); // удаление элемента с головы
    void removeFromTheEnd(); // удаление элемента с хвоста
    void removeByValue(std::string value); // удаление элемента по значению
    void existByValue(std::string value); // поиск элемента по значению
    Node* searchByValue(std::string value); // возвращаем ссылку на элемент по значению
    void display();
    void clear();
};

struct DBtable{ // dbTableManager для 1 таблицы
    std::string tableName; // название таблицы
    LinkedList columnName; // название колонки
    int tuples;
};

struct UniversalNode {    
    DBtable data;
    UniversalNode* next;
    UniversalNode* prev;

    UniversalNode(const DBtable& value, UniversalNode* nextNode = nullptr, UniversalNode* prevNode = nullptr)
        : data(value), next(nextNode), prev(prevNode) {
    }
};

struct UniversalLinkedList{ 
    UniversalNode* head;
    UniversalNode* tail;

    UniversalLinkedList() : head(nullptr), tail(nullptr) {}

    void addToTheHeadUni(DBtable value); // добавление элемента в голову
    void addToTheEndUni(DBtable value); // добавление элемента в хвост
    void removeFromTheHeadUni(); // удаление элемента с головы
    void removeFromTheEndUni(); // удаление элемента с хвоста
    void removeByValueUni(DBtable value); // удаление элемента по значению
    void existByValueUni(DBtable value); // поиск элемента по значению
    UniversalNode* searchByValueUni(DBtable value); // возвращаем ссылку на элемент по значению
    void displayUni();
    
};

struct DatabaseManager { // dbManager для всех существующих
    std::string schemaName; // название схемы
    int tuplesLimit; // ограничение
    UniversalLinkedList tables; // таблицы
};

void loadSchema(DatabaseManager& dbManager, const std::string& configPath);

void createCSVFile(const std::string& tableDir, DBtable& table, int tuplesLimit);

void createPrimaryKeyFile(const std::string& tableDir, const std::string& tableName);

void createLockFile(const std::string& tableDir, const std::string& tableName);

void QueryManager(const DatabaseManager& dbManager, DBtable& table);

void createDirectoriesAndFiles(const DatabaseManager& dbManager);
