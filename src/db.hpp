#ifndef DB_HPP_
#define DB_HPP_
#include "sqlite3.h"
#include <QString>

class DB {

public:
    virtual void init() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void exec(const QString&) = 0;
};



class SqlDB : public DB {

public:
    SqlDB(QString path);
    // SqlDB& operator=(SqlDB&&)
    void init();
    void connect();
    void disconnect();
    void exec(const QString &statement);
private:
    sqlite3 *db;
    const QString path;
};

// Maybe rework this at some point to expose a global const dbPath instead of...
// whatever it does right now.
static const QString logFileName = "writelog.db";
//static QString dbPath;

extern QString dbPath();
// static QString dbPath();
// const QString = dbPath();
// extern SqlDB globalDB;

#endif
