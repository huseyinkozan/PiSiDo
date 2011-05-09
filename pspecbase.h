#ifndef PSPECBASE_H
#define PSPECBASE_H

#include <QString>
#include <QMap>

class PSpecBase
{
public:
    PSpecBase();

    enum VersionReleaseToFromAttr {
        VERSIONFROM,
        VERSIONTO,
        VERSION,
        RELEASEFROM,
        RELEASETO,
        RELEASE
    };
    enum AdditionalFileAttr {
        PERMISSION,
        OWNER,
        GROUP
    };

    QString name;
    QList<QString> license;                                     // one or more
    QString part_of;
    QList<QString> is_a_s;                                      // zero or more
    QMap<QString,QString> summaries;                            // en=bla bla, tr=hede hede
    QMap<QString,QString> descriptions;                         // en=bla bla, tr=hede hede
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > build_dependencies;  // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<AdditionalFileAttr,QString> > additional_files;     // additional_file, {(permission,xxxx),(owner,xxxx),(group,xxxx)}

};

#endif // PSPECBASE_H
