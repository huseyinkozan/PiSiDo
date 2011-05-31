#ifndef PISISOURCE_H
#define PISISOURCE_H

#include <QString>
#include <QMap>

#include "pisispbase.h"

class QDomElement;

class PisiSource : public PisiSPBase
{
public:
    PisiSource();

    enum ArchiveAttr {
        SHA1SUM,
        TYPE,
        TARGET
    };

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);
    virtual void save_to_dom(QDomElement & dom_element);

    QString get_home_page() const;
    QMap<QString,QString> get_packager() const;
    QMap<QString, QMap<ArchiveAttr,QString> > get_archives() const;

    void set_home_page(QString home_page);
    void set_packager(QMap<QString,QString> packager);
    void set_archives(QMap<QString, QMap<ArchiveAttr,QString> > archives);

    bool operator ==(const PisiSource & other);
    bool operator !=(const PisiSource & other);

private:
    ArchiveAttr get_archive_attr_property(QString attr_name);

private:
    QString home_page;
    QMap<QString,QString> packager;                         // name=ali veli, email=q@q.com
    QMap<QString, QMap<ArchiveAttr,QString> > archives;     // archive, {(sha1sum,xxxx),(type,xxxx),(target,xxxx)}
};

#endif // PISISOURCE_H
