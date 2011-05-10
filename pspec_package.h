#ifndef PSPECPACKAGE_H
#define PSPECPACKAGE_H

#include "pspecbase.h"

class PSpecPackage : public PSpecBase
{
public:
    PSpecPackage();

    enum FileTypes {
        EXECUTABLE,
        LIBRARY,
        DATA,
        CONFIG,
        DOC,
        MAN,
        INFO,
        LOCALEDATA,
        HEADER,
        ALL
    };

    QMap<QString, FileTypes> files;   // path, {(fileType,xxx),(permanent,xxx)}
    QMap<QString, bool> permanent_files;    // only in the list if defined !
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;  // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_anydependencies;  // anydependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
};

#endif // PSPECPACKAGE_H
