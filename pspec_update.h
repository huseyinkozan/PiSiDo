#ifndef PSPECUPDATE_H
#define PSPECUPDATE_H

#include <QString>
#include <QDate>

class QDomElement;

class PSpecUpdate
{
public:
    PSpecUpdate();

    void clear();
    void load_from_dom(const QDomElement & dom_element);

    enum UpdateType { UNDEFINED, CRITICAL, SECURITY };

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;
};

#endif // PSPECUPDATE_H
