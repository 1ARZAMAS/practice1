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


struct Array { // done
    Node* head; // указываем на первый узел
    int size; //текущий размер списка
    int capacity; // максимальная вместимость массива
    
    Array(size_t cap = 15) : size(0), capacity(cap) { // конструктор для создания
        array = new string[capacity]; // выделение памяти
    }

    ~Array() {
        delete[] array; // освобождение памяти
    }

    void resize();
    void add(int index, std::string value);
    void addToTheEnd(std::string value);
    void get(int index);
    void remove(int index);
    void replace(int index, std::string value);
    void length();
    void display();
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);
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
    int HashFun(const std::string& key);
    HashTableItem* createItem(const std::string& key, std::string data);
    void push(const std::string& key, std::string data);
    HashTableItem* get(const std::string& key) const;
    void pop(const std::string& key);
};

struct DatabaseManager {
    std::string schemaName;
    int tuplesLimit;
    LinkedList tables;
    HashTable hashTable;  // Хеш-таблица для хранения таблиц и колонок
};

void loadSchema(DatabaseManager& dbManager, const std::string& configPath);

void createCSVFile(const std::string& tableDir, const std::string& tableName, const DatabaseManager& dbManager);

void createPrimaryKeyFile(const std::string& tableDir, const std::string& tableName);

void createLockFile(const std::string& tableDir, const std::string& tableName);

//void loadSchema(const std::string& filePath);

void QueryManager(const DatabaseManager& dbManager);

void createDirectoriesAndFiles(const DatabaseManager& dbManager);
