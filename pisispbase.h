#ifndef PISISPBASE_H
#define PISISPBASE_H

#include <QString>
#include <QStringList>
#include <QMap>

class QDomElement;

class PisiSPBase
{
public:
    PisiSPBase();

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
    virtual void save_to_dom(QDomElement & dom_element);

    virtual QStringList get_dependency_list(QMap<QString, QMap<VersionReleaseToFromAttr,QString> > dependencies);
    virtual QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_dependency_list(QString dependency);

    QString get_name() const;
    QString get_summary() const;
    QString get_description() const;
    QString get_part_of() const;
    QString get_license() const;
    QString get_is_a() const;
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_build_dependencies() const;

    void set_name(QString name);
    void set_summary(QString summary);
    void set_description(QString description);
    void set_part_of(QString part_of);
    void set_license(QString license);
    void set_is_a(QString is_a);
    void set_build_dependencies(QMap<QString, QMap<VersionReleaseToFromAttr,QString> > build_dependencies);
    void set_build_dependencies(QString build_dependency_string);

    bool operator ==(const PisiSPBase & other) const;
    bool operator !=(const PisiSPBase & other) const;

protected:
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > get_dependency_map(QDomElement elm, bool mandatory);
    VersionReleaseToFromAttr get_dependency_attr_property(QString attr_name, bool abbreviation = false);
    QString get_dependency_attr_property_string(VersionReleaseToFromAttr attr, bool abbreviation = false);

private:
    QString get_value_from_element(QString tag, QDomElement elm, bool mandatory);
    QMap<VersionReleaseToFromAttr,QString> get_dependency_attr_list(QString attr_string);

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

#endif // PISISPBASE_H
