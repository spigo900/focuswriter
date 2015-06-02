#include "db.hpp"
class QString;
class QDateTime;
class Document;



class Logger {

public:
    Logger();
    // TODO: Figure out a way to do this that doesn't involve the dreaded Global
    // State.
    static qint64 timeSinceFirstAccess;
    void logUpdate(Document newDoc);
// public slots:
//     void logTabChanged(int newIndex);
private:
    int lastIndex;
    int wordsAtStart;
    QDateTime docAccessed;
    DB database;
    QString lastDocTitle;
}
