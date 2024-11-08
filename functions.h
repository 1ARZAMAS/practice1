#pragma once
#include <string>

bool tableExists(const DatabaseManager& dbManager, const std::string& tableName);

void splitPoint(LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, std::string& wordFromQuery);

std::string cleanString(const std::string& str);

int amountOfCSV(const DatabaseManager& dbManager, const std::string& tableName);

void copyFirstRow(std::string& firstTable, std::string& tableDir);