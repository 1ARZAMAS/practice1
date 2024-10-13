#include <string>
#include <iostream>
#include "header.h"

using namespace std;

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
    current->next = nullptr; // обнуляем указатель на последний элемент те разрываем связь с последним узлом
   
    delete tail; // удаляем последний узел
    tail = current; // конец теперь указывает на последний элемент, предпоследний узел
}

void UniversalLinkedList::removeByValueUni(DBtable value){ // удаление элемента по значению
    if (head == nullptr){
        cout << "Невозможно удалить элемент: список пуст" << endl;
        return;
    }
    if (value.tableName == head->data.tableName){
        removeFromTheHeadUni();
        return;
    }
    if (value.tableName == tail->data.tableName){
        removeFromTheEndUni();
        return;
    }
    UniversalNode* current = head;
    while (current->next && current->next->data.tableName != value.tableName){ // Пока вообще можем идти по списку и пока значение не будет равно нужному
   
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
    while (current->next && current->data.tableName != value.tableName) {
        current = current->next;
    }
    if (current->data.tableName == value.tableName){
        cout << "Значение " << current->data.tableName << " существует в списке" << endl;
    } else {
        cout << "Такого элемента " << current->data.tableName << " нет в списке" << endl;
    }
}

UniversalNode* UniversalLinkedList::searchByValueUni(DBtable value){
    UniversalNode* current = head;
    while (current->next && current->data.tableName != value.tableName) {
        current = current->next;
    }
    if (current->data.tableName == value.tableName){
        return current;
    } else {
        return nullptr;
    }
}

void UniversalLinkedList::displayUni(){
    UniversalNode* current = head;
    while (current != nullptr) {
        cout << current->data.tableName << " ";
        current = current->next;
    }
    cout << endl;
}