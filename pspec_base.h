#ifndef PSPECBASE_H
#define PSPECBASE_H

#include <QString>
#include <QMap>

class QDomElement;

class PSpecBase
{
public:
    PSpecBase();

    virtual void clear();

    virtual void load_from_dom(const QDomElement & dom_element);

    enum VersionReleaseToFromAttr {
        VERSIONFROM,
        VERSIONTO,
        VERSION,
        RELEASEFROM,
        RELEASETO,
        RELEASE
    };

    QString name;
    QString summary;                                            // only english !
    QString description;                                        // only english !
    QString part_of;
    QString license;                                     // one or more, only first !
    QString is_a;                                      // zero or more, only first !
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > build_dependencies;  // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}

private:
    QString get_value_from_element(QString tag, QDomElement elm, bool mandatory);
    VersionReleaseToFromAttr get_dependency_attr_property(QString attr_name);
};

#endif // PSPECBASE_H
