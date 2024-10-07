#include "json.hpp"

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
    void searchByValue(std::string value); // поиск элемента по значению
    void display();
};

struct DatabaseManager {
    std::string schemaName;
    int tuplesLimit;
    LinkedList tables;
};

const int SIZE = 500;

struct HashTableItem {
    std::string key;
    std::string data;
    HashTableItem* next; // Указатель на следующий элемент в цепочке
};

struct HashTable {
    
    HashTableItem* items[SIZE];
    int count;

    HashTable() : count(0) {//заполнение таблицы
        for (int i = 0; i < SIZE; i++) { // все элементы nullptr
            items[i] = nullptr;
        }
    }
    int HashFun(const std::string& key) const;
    HashTableItem* createItem(const std::string& key, std::string data);
    void push(const std::string& key, std::string data);
    std::string get(const std::string& key) const;
    void pop(const std::string& key);
};

std::string join(const json& columns);

void loadSchema(DatabaseManager& dbManager, const std::string& filePath, HashTable& schemaHashTable);

void createDirectoriesAndFiles(const DatabaseManager& dbManager, const HashTable& schemaHashTable);

void createPrimaryKeyFile(const std::string& tableDir, const std::string& tableName);

void createCSVFile(const std::string& tableDir, const std::string& tableName, const HashTable& schemaHashTable);

void createLockFile(const std::string& tableDir, const std::string& tableName);

void QueryManager(const DatabaseManager& dbManager);