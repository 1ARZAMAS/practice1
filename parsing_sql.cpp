// Парсит и выполняет SQL-запросы
void QueryManager(const string& qr, const string& fileDirectory, const string& schemaName, const int tuplesLimit, const HashMap<string,DynamicArray<string>*>&JSONSchema) {  
    if (qr.find("SELECT") != -1) {
        int index = qr.find("FROM") - 1;
        str::string columns = qr.substr(7, index - 7);
        int index2 = qr.find("WHERE");
        if ( index2 != -1){
            str::string tables = qr.substr(index, index2 - 1);
            str::string filter = qr.substr(index2 + 6);
        } else {
            str::string tables = qr.substr(index);
        }
        
    }
    // else if (qr.find("INSERT") != -1) {
    //     int index = qr.find("INTO") - 1;
    //     str::string tables = qr.substr(7, index - 7);
    //     int index2 = qr.find("VALUES");
    //     if ( index2 != -1){
    //         str::string tables = qr.substr(index, index2 - 1);
    //         str::string filter = qr.substr(index2 + 6);
    //     } else {
    //         str::string tables = qr.substr(index);
    //     }
    //     try{
    //         parsingInsert(*qr, fileDirectory, schemaName, tuplesLimit, JSONSchema);
    //     } catch (const exception& ErrorInfo) {
    //         cerr << ErrorInfo.what () << endl;
    //     }
    // }  
    // else if (qr.find("DELETE") != -1) {
    //     int index = qr.find("FROM") - 1;
    //     str::string columns = qr.substr(7, index - 7);
    //     int index2 = qr.find("WHERE");
    //     if ( index2 != -1){
    //         str::string tables = qr.substr(index, index2 - 1);
    //         str::string filter = qr.substr(index2 + 6);
    //     } else {
    //         str::string tables = qr.substr(index);
    //     }
    //     try {
    //         parsingDelete(*qr, fileDirectory, schemaName, JSONSchema);
    //     } catch (const exception& ErrorInfo) {
    //         cerr << ErrorInfo.what () << endl;
    //     }
    //} 
    else {
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

// void parsingInsert(){
// }
// void parsingDelete(){
// }
