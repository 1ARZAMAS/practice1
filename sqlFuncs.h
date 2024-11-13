#pragma once
#include <string>

void insertFunc(const DatabaseManager& dbManager, const std::string& tableName, std::string& query, int& currentKey);

void deleteFunc(const DatabaseManager& dbManager, const std::string& tableName, std::string& query, LinkedList& tableFromQuery, LinkedList& columnFromQuery);

void crossJoin(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& tableName, LinkedList& columnsFromQuery);

void selectWithWhere(int& fileCountFirstTable, int& fileCountSecondTable, const DatabaseManager& dbManager, const std::string& query, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery);
