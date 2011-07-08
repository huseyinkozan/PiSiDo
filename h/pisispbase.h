#ifndef PISISPBASE_H
#define PISISPBASE_H

#include <QString>
#include <QStringList>
#include <QMap>

class QDomElement;
class QDomText;

class PisiSPBase
{
public:
    PisiSPBase();

    enum VRTFAttr {
        VERSIONFROM,
        VERSIONTO,
        VERSION,
        RELEASEFROM,
        RELEASETO,
        RELEASE
    };
    enum AFileAttr {
        TARGET,
        PERMISSION,
        OWNER,
        GROUP
    };

    virtual void clear();
    virtual void load_from_dom(const QDomElement & root);
    virtual void save_to_dom(QDomElement & root);

    virtual QStringList get_dependency_list(QMap<QString, QMap<VRTFAttr,QString> > dependencies);
    virtual QMap<QString, QMap<VRTFAttr,QString> > get_dependency_list(QString dependency);

    QString get_name() const;
    QString get_summary() const;
    QString get_description() const;
    QString get_part_of() const;
    QString get_license() const;
    QString get_is_a() const;
    QMap<QString, QMap<VRTFAttr,QString> > get_build_dependencies() const;
    QMap<QString, QMap<AFileAttr,QString> > get_aditional_files() const;

    AFileAttr get_aditional_file_attribute(QString attr_name);
    QString get_aditional_file_attribute(AFileAttr attr);

    void set_name(QString name);
    void set_summary(QString summary);
    void set_description(QString description);
    void set_part_of(QString part_of);
    void set_license(QString license);
    void set_is_a(QString is_a);
    void set_build_dependencies(QMap<QString, QMap<VRTFAttr,QString> > build_dependencies);
    void set_build_dependencies(QString build_dependency_string);
    void set_aditional_files(QMap<QString, QMap<AFileAttr,QString> > aditional_files);

    bool operator ==(const PisiSPBase & other) const;
    bool operator !=(const PisiSPBase & other) const;

protected:
    QDomElement append_element(QDomElement & root, QString tag);
    QDomText append_text_element(QDomElement root, QString value);

    QString get_element_value(QDomElement root, QString tag);
    QDomElement set_element_value(QDomElement root, QString tag, QString value);

    virtual bool is_mandatory(QDomElement root, QString tag);

    QMap<QString, QMap<VRTFAttr,QString> > get_dependency(QDomElement elm);
    void set_dependency(QDomElement root, QMap<QString, QMap<VRTFAttr,QString> > dep);

    VRTFAttr get_dependency_attribute(QString attr_name, bool abbreviation = false);
    QString get_dependency_attribute(VRTFAttr attr, bool abbreviation = false);

    QMap<QString, QMap<AFileAttr,QString> > get_aditional_file(QDomElement elm);
    void set_aditional_file(QDomElement root, QMap<QString, QMap<AFileAttr,QString> > a_files);

private:
    QMap<VRTFAttr,QString> get_dependency_attr_list(QString attr_string);

private:
    QString name;
    QString summary;                                            // only english !
    QString description;                                        // only english !
    QString part_of;
    QString license;                                     // one or more, only first !
    QString is_a;                                      // zero or more, only first !
    // dependency, {(versionFrom,xxxx),(versionTo,xxxx),...,(release,xxxx)}
    QMap<QString, QMap<VRTFAttr,QString> > build_dependencies;
    // file, {(target:xxxx),(permission:xxx),(owner:xxx),(group:xxx)}
    QMap<QString, QMap<AFileAttr,QString> > aditional_files;
};

#endif // PISISPBASE_H
