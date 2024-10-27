#include <iostream>
#include <fstream>
#include <cstddef>
#include <string>
#include <sstream>
#include "header.h"

using namespace std;

void QueryManager(const DatabaseManager& dbManager) {
    string command;
    while(true){
        cout << "< ";
        getline(cin, command);
        istringstream iss(command);
        string wordFromQuery;
        iss >> wordFromQuery; // первое слово в команде
         
        if (wordFromQuery == "exit"){
            return;
        } else if (wordFromQuery == "SELECT"){
            try {
                cout << "sel" << endl;

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "DELETE"){
            try {
                cout << "del" << endl; 

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else if (wordFromQuery == "INSERT"){
            try {
                cout << "ins" << endl;

            } catch (const exception& ErrorInfo) {
                cerr << ErrorInfo.what() << endl;
            }
        } else {
            cerr << "Incorrect SQL query" << endl;
        }
    }
}