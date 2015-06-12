#include "sql_logger.hpp"
#include "db.hpp"

#include <QModelIndex>
#include <QDateTime>
#include <QString>
#include <string>
#include <QDir>

#include "document.h"



static const QString setup =
    "CREATE TABLE IF NOT EXISTS FileTracking(Id INTEGER PRIMARY KEY, title TEXT, timestamp TEXT, words INT, wordDelta INT);";
static const int wordsIndex = 1;
static const int timeIndex = 2;

// Qt::DisplayRole is the appropriate role for accessing the stats
// QModelIndex idxTime = QModelIndex(...);
// QModelIndex idxWords = QModelIndex(...);

// Is there any way I can get the time actually spent writing out of the
// DailyProgress object? I should look into that.

static void logData(DB* db, QString title, QDateTime time, int words, int wordDelta = 0)
{
    QString sqlcmd = QString("INSERT INTO FileTracking(title, timestamp, words, wordDelta) "
                             "VALUES('%1', '%2', %3, %4);")
        .arg(
            title,
            time.toString(),
            QString::number(words),
            QString::number(wordDelta));
    db->exec(sqlcmd);
}

// Set up the logger, connect to the database and make sure the schema works.
Logger::Logger() : database(dbPath())
{
    // SqlDB database(dbPath());
    // this->database = SqlDB(dbPath());
    this->database.init();
    this->database.connect();
    this->database.exec(setup);
}

// Called when a file is saved. Should log the time, title, absolute wordcount
// and wordcount delta.
void Logger::logUpdate(Document* doc, int wordDelta)
{
    // TODO: Make this actually write to the appropriate table.
    QDateTime now = QDateTime::currentDateTimeUtc();
    logData(&database, doc->title(), now, doc->wordCount(), wordDelta);
}

// Called when a file is accessed. Should like the time, title and absolute
// wordcount.
void Logger::logAccess(Document* newDoc)
{
    // TODO: Make this actually write to the appropriate table.
    QDateTime now = QDateTime::currentDateTimeUtc();
    logData(&database, newDoc->title(), now, newDoc->wordCount());
}

Logger logger;
