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

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;

    int release;

private:
    QString get_value_from_element(QString tag, QDomElement elm, bool mandatory);
};

#endif // PSPECUPDATE_H
