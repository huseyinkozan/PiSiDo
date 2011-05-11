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

    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;  // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
};

#endif // PSPECPACKAGE_H
