#pragma once
#include <string>

bool isLocked(const DatabaseManager& dbManager, const std::string& tableName);

void locking(const DatabaseManager& dbManager, const std::string& tableName);

void unlocking(const DatabaseManager& dbManager, const std::string& tableName);