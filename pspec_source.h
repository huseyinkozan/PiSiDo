#ifndef PSPECSOURCE_H
#define PSPECSOURCE_H

#include <QString>
#include <QMap>

#include "pspecbase.h"

class PSpecSource : public PSpecBase
{
public:
    PSpecSource();


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
