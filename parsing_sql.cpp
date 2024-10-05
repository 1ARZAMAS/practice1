// Парсит и выполняет SQL-запросы
void QueryManager(const string& qr, const string& fileDirectory, const string& schemaName, const int tuplesLimit, const HashMap<string,DynamicArray<string>*>&JSONSchema) {  
    if (qr.find("SELECT") != -1) {
        qr.substr(12);
        try {
            parseSelect(*qr, fileDirectory, schemaName, JSONSchema);
        } catch (const exception& ErrorInfo) {
            cerr << Errorinfo.what () << endl;
        }
    }
    else if (qr-›data[0] == "INSERT" && qr-›data[1] == "INTO") {
        try{
            parsingInsert(*qr, fileDirectory, schemaName, tuplesLimit, JSONSchema);
        } catch (const exception& ErrorInfo) {
            cerr << ErrorInfo.what () << endl;
        }

    }  
    else if (qr-›data[0] == "DELETE" && qr-›data[1] == "FROM") {
        try {
            parsingDelete(*qr, fileDirectory, schemaName, JSONSchema);
        } catch (const exception& ErrorInfo) {
            cerr << ErrorInfo.what () << endl;
        }

    } else {
        cout << "Invalid SQL query" << endl;
    }
} 
// void executeQuery(const std::string& query) {
//         if (query.find("SELECT")) {
//             parseSelect(*qr, fileDirectory, schemaName, cshema);
//         } else if (query.find("INSERT INTO")) {
//             parseInsert(query);
//         } else if (query.find("DELETE FROM")) {
//             parseDelete(query);
//         } else {
//             throw std::invalid_argument("Unsupported query.");
//         }
//     }
void parseSelect(HashTable::*qr, fs::fileDirectory, std::string schemaName, cshema){

}

void parsingInsert(){

}

void parsingDelete(){

}
