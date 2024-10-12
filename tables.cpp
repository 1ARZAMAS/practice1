#include <string>
#include <iostream>
#include "header.h"

using namespace std;

struct DBtable{
    std::string tableName;
    LinkedList columnName;
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

void UniversalLinkedList::addToTheHeadUni(DBtable value){ // Добавление в самое начало
    UniversalNode* newNode = new UniversalNode(value);
    if (head == nullptr){
        head = tail = newNode;
    } else {
        newNode->next = head;
        head = newNode;
    }
}

void UniversalLinkedList::addToTheEndUni(DBtable value){
    UniversalNode* newNode = new UniversalNode(value);
    if (head == nullptr){
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

void UniversalLinkedList::removeFromTheHeadUni(){// удаление элемента с головы
    if (head == nullptr){
        cout << "Удаление невозможно: список пустой" << endl;
        return;
    } else{
        UniversalNode* temp = head;
        head = head->next;
        delete temp;
    }
}

void UniversalLinkedList::removeFromTheEndUni(){// удаление элемента с хвоста
    if (head == nullptr){
        cout << "Удаление невозможно: список пустой" << endl;
        return;
    }
    if (head == tail){
        delete head;
        head = nullptr;
        tail = nullptr;
        return;
    }
    UniversalNode* current = head;
    while (current->next != tail){ // текущий будет указывать на предпоследний узел
        current = current->next;
    }
    current->next = nullptr; // обнуляем указатель на последний элемент
    // те разрываем связь с последним узлом
    delete tail; // удаляем последний узел
    tail = current; // конец теперь указывает на последний элемент, предпоследний узел
}

void UniversalLinkedList::removeByValueUni(DBtable value){ // удаление элемента по значению
    if (head == nullptr){
        cout << "Невозможно удалить элемент: список пуст" << endl;
        return;
    }
    if (value == head->data){
        removeFromTheHead();
        return;
    }
    if (value == tail->data){
        removeFromTheEnd();
        return;
    }
    UniversalNode* current = head;
    while (current->next && current->next->data != value){ // Пока вообще можем идти по списку
    // и пока значение не будет равно нужному
        current = current->next;
    }
    if (current->next == nullptr){
        cout << "Такого значения нет в списке" << endl;
        return;
    }
    UniversalNode* temp = current->next;
    current->next = temp->next; // Обновляем указатель на следующий элемент
    delete temp; // Удаляем узел
}

void UniversalLinkedList::existByValueUni(DBtable value){ // поиск элемента по значению
    UniversalNode* current = head;
    while (current->next && current->data != value) {
        current = current->next;
    }
    if (current->data == value){
        cout << "Значение " << current->data << " существует в списке" << endl;
    } else {
        cout << "Такого элемента " << current->data << " нет в списке" << endl;
    }
}

UniversalNode* UniversalLinkedList::searchByValueUni(DBtable value){
    UniversalNode* current = head;
    while (current->next && current->data != value) {
        current = current->next;
    }
    if (current->data == value){
        return current;
    } else {
        cout << "Такого элемента " << current->data << " нет в списке" << endl;
        return nullptr;
    }
}

void UniversalLinkedList::displayUni(){
    UniversalNode* current = head;
    while (current != nullptr) {
        cout << current->data << " ";
        current = current->next;
    }
    cout << endl;
}