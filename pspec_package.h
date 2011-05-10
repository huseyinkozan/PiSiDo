#ifndef PSPECPACKAGE_H
#define PSPECPACKAGE_H

#include "pspec_base.h"

class QDomDocumentFragment;

class PSpecPackage : public PSpecBase
{
public:
    PSpecPackage();

    virtual void clear();
    virtual bool load_from_dom(const QDomDocumentFragment & dom_fragment);

    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;  // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}

    /* files : unused */
//    enum FileTypes {
//        EXECUTABLE,
//        LIBRARY,
//        DATA,
//        CONFIG,
//        DOC,
//        MAN,
//        INFO,
//        LOCALEDATA,
//        HEADER,
//        ALL
//    };
//    QMap<QString, FileTypes> files;   // path, {(fileType,xxx),(permanent,xxx)}
//    QMap<QString, bool> permanent_files;    // only in the list if defined !

/* runtime_anydependencies : unused */
//    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_anydependencies;  // anydependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
};

#endif // PSPECPACKAGE_H
