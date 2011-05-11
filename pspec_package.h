#ifndef PSPECPACKAGE_H
#define PSPECPACKAGE_H

#include "pspec_base.h"

class QDomElement;

class PSpecPackage : public PSpecBase
{
public:
    PSpecPackage();

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);

    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;
};

#endif // PSPECPACKAGE_H
