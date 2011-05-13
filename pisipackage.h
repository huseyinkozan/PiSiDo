#ifndef PISIPACKAGE_H
#define PISIPACKAGE_H

#include "pisispbase.h"

class QDomElement;

class PisiPackage : public PisiSPBase
{
public:
    PisiPackage();

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);

    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_runtime_dependencies();

private:
    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;
};

#endif // PISIPACKAGE_H
