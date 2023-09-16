
#include "sqlite/sqlite3.h"
#ifndef DATABASE_H_
#define DATABASE_H_

class Database  {
public:
    Database(const char *database);
    sqlite3 *db;
    
};


#endif