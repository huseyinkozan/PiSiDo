#ifndef PISIUPDATE_H
#define PISIUPDATE_H

#include <QString>
#include <QDate>

class QDomElement;

class PisiUpdate
{
public:
    PisiUpdate();

    void clear();
    void load_from_dom(const QDomElement & dom_element);

    int get_release();
    QString get_version();
    QString get_comment();
    QString get_packager_name();
    QString get_packager_email();

private:
    QString get_value_from_element(QString tag, QDomElement elm, bool mandatory);

private:
    int release;

    QDate date;
    QString version;
    QString comment;
    QString packager_name;
    QString packager_email;
};

#endif // PISIUPDATE_H
