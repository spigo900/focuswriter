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
    void init();
    void connect();
    void disconnect();
    void exec(const QString &statement);
private:
    sqlite3 *db;
    const QString path;
};

static const logFileName = "writelog.db"
static QString dbPath;

#if defined(Q_OS_MAC)
  dbPath = QDir::homePath() + "/Library/Application Support/GottCode/FocusWriter/" + logFileName;
#elif defined(Q_OS_UNIX)
  dbPath = QString::fromLocal8Bit(qgetenv("XDG_DATA_HOME"));
  if (dbPath.isEmpty()) {
      dbPath = QDir::homePath() + "/.local/share/";
  }
  dbPath += ("/focuswriter/" + logFileName);
#else
  dbPath = QDir::homePath() + "/Application Data/GottCode/FocusWriter/" + logFileName;
#endif

SqlDB globalDB = SqlDB(dbPath);
