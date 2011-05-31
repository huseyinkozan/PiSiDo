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
    void save_to_dom(QDomElement & dom_element);

    int get_release() const;
    QDate get_date() const;
    QString get_version() const;
    QString get_comment() const;
    QString get_packager_name() const;
    QString get_packager_email() const;

    void set_release(int r);
    void set_date(QDate d);
    void set_version(QString v);
    void set_comment(QString c);
    void set_packager_name(QString p_n);
    void set_packager_email(QString p_e);

    bool operator ==(const PisiUpdate & other);
    bool operator !=(const PisiUpdate & other);
    bool operator <(const PisiUpdate & other);
    bool operator >(const PisiUpdate & other);

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
