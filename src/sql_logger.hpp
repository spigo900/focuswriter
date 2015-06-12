#ifndef LOGGER_HPP_
#define LOGGER_HPP_

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
    void logUpdate(Document* doc, int wordDelta);
    void logAccess(Document* newDoc);
// public slots:
//     void logTabChanged(int newIndex);
private:
    SqlDB database;
};

extern Logger logger;

#endif
