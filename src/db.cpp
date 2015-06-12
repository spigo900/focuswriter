#include "db.hpp"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <sqlite3.h>
#include <assert.h>
#include <iostream>
// #include <string>
// #include <cstdio>

// using std::string;
// using std::cout;



SqlDB::SqlDB(QString path) : path(path)
{
    db = NULL;
};

void SqlDB::init()
{
    QFileInfo dbFileInfo = QFileInfo(path);
    QString dbFilePath = dbFileInfo.filePath();
    QDir dbPath = QDir(dbFilePath);
    bool successPath = dbPath.cdUp();
    assert(successPath);

    if (!dbPath.exists()) {
        bool ret = dbPath.mkpath(dbPath.path());
        assert(ret);
    };
    if (!dbFileInfo.exists()) {
        QFile file(dbFilePath);
        file.open(QIODevice::WriteOnly);
        file.close();
        return;
    };
    // If the DB file is either a directory or unreadable, we're fucked, so
    // crash and burn, baby.
    assert(dbFileInfo.isFile() && dbFileInfo.isReadable());
}

void SqlDB::connect()
{
    QByteArray temp = this->path.toLatin1();
    const char *tmppath = temp.data();
    sqlite3_open(tmppath, &this->db);
}

void SqlDB::disconnect()
{
    sqlite3_close(this->db);
}

void SqlDB::exec(const QString & statement)
{
    QByteArray temp = statement.toLatin1();
    const char *sql = temp.data();
    // printf(sql);
    // printf("\n");
    // char **error;
    // int t =
    // sqlite3_exec(this->db, sql, NULL, NULL, error);
    sqlite3_exec(this->db, sql, NULL, NULL, NULL);
    // cout << string(*error);
    // printf(*error);
    // sqlite3_free(error);
}

QString dbPath()
{
#if defined(Q_OS_MAC)
    QString dbPath = QDir::homePath() + "/Library/Application Support/GottCode/FocusWriter/" + logFileName;
#elif defined(Q_OS_UNIX)
    QString dbPath = QString::fromLocal8Bit(qgetenv("XDG_DATA_HOME"));
    if (dbPath.isEmpty()) {
        dbPath = QDir::homePath() + "/.local/share/";
    }
    dbPath += ("/focuswriter/" + logFileName);
#else
    QString dbPath = QDir::homePath() + "/Application Data/GottCode/FocusWriter/" + logFileName;
#endif
    return dbPath;
}
