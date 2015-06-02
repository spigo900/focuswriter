#include "db.hpp"
#include <QFile>
#include <QFileInfo>
#include <QDir>



void SqlDB::SqlDB(string path)
{
    this.path = QString(path);
    this.db = NULL;
};

void SqlDB::init()
{
    QFileInfo dbFileInfo = QFileInfo(this.path);
    QString dbFilePath = dbFileInfo.filePath()
    QDir dbDir = QDir(dbFilePath);

    if (!dbDir.exists()) {
        bool ret = QDir::mkpath(dbFilePath);
    };
    if (!dbFileInfo.exists()) {
        QFile file = QFile::open(QIODevice::WriteOnly);
        file.close();
        return;
    };
    // If the DB file is either a directory or unreadable, we're fucked, so
    // crash and burn, baby.
    assert(dbFileInfo.isFile() && dbFileInfo.isReadable())
};

void SqlDB::connect()
{
    QByteArray temp = this.path.toLatin();
    const char *tmppath = temp.data();
    sqlite3_open(tmppath, &this.db);
};

void SqlDB::disconnect()
{
    sqlite3_close(this.db);
};

void SqlDB::exec(const QString & statement)
{
    QByteArray temp = statement.toLatin();
    const char *sql = temp.data();
    sqlite3_exec(this.db, sql, NULL, NULL);
};
