#include "sql_logger.hpp"

#include <QDateTime>
#inlcude <QString>



// void Logger::logTabChanged(int newIndex)
// {
//     // TODO: Make this actually write to the appropriate table.

//     // First part should log the end of the editing for the file, along with any
//     // appropriate values -- time spent, word count delta, whatever.
//     // this.database.exec()

//     // Second should change the last-accessed index to the new one and start
//     // logging new values.
//     this.lastIndex = newIndex;
//     this.docAccessed = QDateTime::currentDateTimeUtc();
//     // this.database.exec()
// };

void logUpdate(Document* newDoc, int newIndex)
{
    // TODO: Make this actually write to the appropriate table.
    QDateTime now = QDateTime::currentDateTimeUtc();

    // First part should log the end of the editing for the file, along with any
    // appropriate values -- time spent, word count delta, whatever.
    QString title = this.lastDocTitle();
    int words = this.wordsAtStart;
    // TODO: Figure out how the hell to determine the current number of words
    // when the last file may not even be open any more.
    QDateTime start = this.docAccessed;
    // End time should be the var 'now,' since there's not really any need to
    // define a separate variable for it.

    // this.database.exec()

    // Second should change the last-accessed index to the new one and start
    // logging new values.
    this.lastDocTitle = newDoc->title();
    this.docAccessed = now;
    this.wordsAtStart = newDoc->wordCount();

    this.lastIndex = newIndex;
};
