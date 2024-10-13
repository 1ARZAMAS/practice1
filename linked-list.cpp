#include <string>
#include <iostream>
#include "header.h"

using namespace std;

void LinkedList::addToTheHead(std::string value){ // Добавление в самое начало
    Node* newNode = new Node(value);
    if (head == nullptr){
        head = tail = newNode;
    } else {
        newNode->next = head;
        head = newNode;
    }
}

void LinkedList::addToTheEnd(std::string value){
    Node* newNode = new Node(value);
    if (head == nullptr){
        head = tail = newNode;
    } else {
        tail->next = newNode;
        tail = newNode;
    }
}

void LinkedList::removeFromTheHead(){// удаление элемента с головы
    if (head == nullptr){
        cout << "Удаление невозможно: список пустой" << endl;
        return;
    } else{
        Node* temp = head;
        head = head->next;
        delete temp;
    }
}

void LinkedList::removeFromTheEnd(){// удаление элемента с хвоста
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
    Node* current = head;
    while (current->next != tail){ // текущий будет указывать на предпоследний узел
        current = current->next;
    }
    current->next = nullptr; // обнуляем указатель на последний элемент
    // те разрываем связь с последним узлом
    delete tail; // удаляем последний узел
    tail = current; // конец теперь указывает на последний элемент, предпоследний узел
}

void LinkedList::removeByValue(std::string value){ // удаление элемента по значению
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
    Node* current = head;
    while (current->next && current->next->data != value){ // Пока вообще можем идти по списку
    // и пока значение не будет равно нужному
        current = current->next;
    }
    if (current->next == nullptr){
        cout << "Такого значения нет в списке" << endl;
        return;
    }
    Node* temp = current->next;
    current->next = temp->next; // Обновляем указатель на следующий элемент
    delete temp; // Удаляем узел
}

void LinkedList::existByValue(std::string value){ // поиск элемента по значению
    Node* current = head;
    while (current->next && current->data != value) {
        current = current->next;
    }
    if (current->data == value){
        cout << "Значение " << current->data << " существует в списке" << endl;
    } else {
        cout << "Такого элемента " << current->data << " нет в списке" << endl;
    }
}

Node* LinkedList::searchByValue(std::string value){
    Node* current = head;
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

void LinkedList::display(){
    Node* current = head;
    while (current != nullptr) {
        cout << current->data << " ";
        current = current->next;
    }
    cout << endl;
}

void LinkedList::clear(){
    while(head){
        removeFromTheHead();
    }
}