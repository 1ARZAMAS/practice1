#include <iostream>
#include "json.hpp"

using namespace nlohmann;
using namespace std;

int Json_parser(){
    std::ifstream file("shema.json");
    nlohmann::json json_data;
    file >> json_data;

    string schema_name = json_data["name"];
    CreateDir(schema_name);

    tuples_limit = json_data["tuples_limit"];

    json schema_structure = json_data["structure"];
    for (auto& [key, value] : schema_structure.items()) {
        //cout << "test 1" << endl;
        CreateDir(schema_name + "/" + key);
        DynamicArray<string>* tempValue = InitializeArray<string>(10, 50); //переписываем не под дин.массив
        //cout << "test 2" << endl;
        
        string colNames = key + "_pk"; //зачем тут _pk
        for (auto columns: value) {
            colNames += ",";
            string temp = columns;
            colNames += temp;
            //cout « "test 3" « endl;
            InsertElement (*tempValue, temp);
        }
        //cout << "test 4" << endl；
        CreateFile(schemaName + "/" + key, "1.csv", colNames, true);
        CreateFile (schemaName + "/" + key, key + "lock.txt", "@", false);
        CreateFile(schemaName + "/" + key, key + "_pk_sequence.txt", "0", false);
        //cout << "test 5" << endl;
        //cout << key << endl;

        InsertElement<string, DynamicArray<string>*>(JSONSchema, key, tempValue);
        //cout << *tempValue << endl;
        //cout<< *RetrieveValue<string, DynamicArray<string>*>(JSONSchema, key) << endl;
    }
    return 0;
}