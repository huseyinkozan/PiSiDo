#ifndef PSPECUPDATE_H
#define PSPECUPDATE_H

#include <QString>
#include <QDate>

class PSpecUpdate
{
public:
    PSpecUpdate();

    enum UpdateType { UNDEFINED, CRITICAL, SECURITY };

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;
};

#endif // PSPECUPDATE_H
