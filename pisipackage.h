#ifndef PISIPACKAGE_H
#define PISIPACKAGE_H

#include "pisispbase.h"

class QDomElement;

class PisiPackage : public PisiSPBase
{
public:
    PisiPackage();

    enum FileType {
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

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);
    virtual void save_to_dom(QDomElement & dom_element);

    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_runtime_dependencies() const;
    QMultiMap<FileType, QString> get_files() const;

    void set_runtime_dependencies(QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies);
    void set_runtime_dependencies(QString runtime_dependency_string);
    void set_files(QMultiMap<FileType, QString> files);

    bool operator ==(const PisiPackage & other);
    bool operator !=(const PisiPackage & other);

private:
    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > runtime_dependencies;
    QMultiMap<FileType, QString> files;
};

#endif // PISIPACKAGE_H
