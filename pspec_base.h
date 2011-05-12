#ifndef PSPECBASE_H
#define PSPECBASE_H

#include <QString>
#include <QStringList>
#include <QMap>

class QDomElement;

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

    virtual void clear();
    virtual void load_from_dom(const QDomElement & dom_element);
    QStringList get_dependency_list(QMap<QString, QMap<VersionReleaseToFromAttr,QString> > dependency);

    QString get_name();
    QString get_summary();
    QString get_description();
    QString get_part_of();
    QString get_license();
    QString get_is_a();
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_build_dependencies();

protected:
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_dependency_map(QDomElement elm, bool mandatory);
    VersionReleaseToFromAttr get_dependency_attr_property(QString attr_name);
    QString get_dependency_attr_property_string(VersionReleaseToFromAttr attr, bool abbreviation = false);

private:
    QString get_value_from_element(QString tag, QDomElement elm, bool mandatory);

private:
    QString name;
    QString summary;                                            // only english !
    QString description;                                        // only english !
    QString part_of;
    QString license;                                     // one or more, only first !
    QString is_a;                                      // zero or more, only first !
    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > build_dependencies;

};

#endif // PSPECBASE_H
