#pragma once
#include <string>

bool tableExists(const DatabaseManager& dbManager, const std::string& tableName);

void splitPoint(LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, std::string& wordFromQuery);

std::string cleanString(const std::string& str);

int amountOfCSV(const DatabaseManager& dbManager, const std::string& tableName);

void copyFirstRow(std::string& firstTable, std::string& tableDir);

bool findDot(std::string str);

std::string getColumnValue(int& fileCountFirstTable, int& fileCountSecondTable, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, const std::string& columnName, int rowIndex1, int rowIndex2, const DatabaseManager& dbManager);

bool recursionFunc(int& fileCountFirstTable, int& fileCountSecondTable, const std::string& query, LinkedList& tablesFromQuery, LinkedList& columnsFromQuery, int rowIndex1, int rowIndex2, const DatabaseManager& dbManager);
