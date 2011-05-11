#ifndef PSPECSOURCE_H
#define PSPECSOURCE_H

#include <QString>
#include <QMap>

#include "pspec_base.h"

class QDomElement;

class PSpecSource : public PSpecBase
{
public:
    PSpecSource();

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);

    enum ArchiveAttr {
        SHA1SUM,
        TYPE,
        TARGET
    };


    QString home_page;
    QMap<QString,QString> packager;                             // name=ali veli, email=q@q.com
    QMap<QString, QMap<ArchiveAttr,QString> > archives;             // archive, {(sha1sum,xxxx),(type,xxxx),(target,xxxx)}
};

#endif // PSPECSOURCE_H
