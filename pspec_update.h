#ifndef PSPECUPDATE_H
#define PSPECUPDATE_H

#include <QString>
#include <QDate>

class QDomDocumentFragment;

class PSpecUpdate
{
public:
    PSpecUpdate();

    void clear();
    bool load_from_dom(const QDomDocumentFragment & dom_fragment);

    enum UpdateType { UNDEFINED, CRITICAL, SECURITY };

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;
};

#endif // PSPECUPDATE_H
