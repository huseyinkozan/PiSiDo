#include "pisispbase.h"

#include <QDomElement>

#include <QTextStream>
#include <QDebug>

PisiSPBase::PisiSPBase()
{
    clear();
}

void PisiSPBase::clear()
{
    name.clear();
    summary.clear();
    description.clear();
    part_of.clear();
    license.clear();
    is_a.clear();
    build_dependencies.clear();
    aditional_files.clear();
}

void PisiSPBase::load_from_dom(const QDomElement & root) throw(QString)
{
    if(root.isNull())
        throw QObject::tr("Dom Element is null while loading to PisiSPBase !");


    name = get_element_value(root, "Name");
    license = get_element_value(root, "License");
    summary = get_element_value(root, "Summary");
    description = get_element_value(root, "Description");
    part_of = get_element_value(root, "PartOf");
    is_a = get_element_value(root, "IsA");

    QDomElement elm = root.firstChildElement("BuildDependencies");
    build_dependencies = get_dependency(elm);
    aditional_files = get_aditional_file(elm);
}

void PisiSPBase::save_to_dom(QDomElement & root) throw(QString)
{
    if(root.isNull())
        throw QObject::tr("Dom Element is null while saving from PisiSPBase to dom !");

    set_element_value(root, "Name", name);
    set_element_value(root, "IsA", is_a);
    set_element_value(root, "PartOf", part_of);
    set_element_value(root, "License", license);
    set_element_value(root, "Summary", summary);
    set_element_value(root, "Description", description);

    if( ! build_dependencies.isEmpty())
    {
        // optional
        QDomElement elm = root.firstChildElement("BuildDependencies");
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = append_element(root, "BuildDependencies");
        set_dependency(elm, build_dependencies);
    }

    if( ! aditional_files.isEmpty())
    {
        // optional
        QDomElement elm = root.firstChildElement("AditionalFiles");
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = append_element(root, "AditionalFiles");
        set_aditional_file(elm, aditional_files);
    }

}

QString PisiSPBase::get_name() const
{
    return name;
}

QString PisiSPBase::get_summary() const
{
    return summary;
}

QString PisiSPBase::get_description() const
{
    return description;
}

QString PisiSPBase::get_part_of() const
{
    return part_of;
}

QString PisiSPBase::get_license() const
{
    return license;
}

QString PisiSPBase::get_is_a() const
{
    return is_a;
}

QStringList PisiSPBase::get_build_dependencies_as_stringlist()
{
    return get_dependency_list(build_dependencies);
}

QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_build_dependencies() const
{
    return build_dependencies;
}

QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > PisiSPBase::get_aditional_files() const
{
    return aditional_files;
}

void PisiSPBase::set_name(QString name) throw(QString)
{
    if(name.isEmpty())
        throw QObject::tr("Empty name !");

    this->name = name;
}

void PisiSPBase::set_summary(QString summary) throw(QString)
{
    if(summary.isEmpty())
        throw QObject::tr("Empty summary !");

    this->summary = summary;
}

void PisiSPBase::set_description(QString description)
{
    // zero or more, no check
    this->description = description;
}

void PisiSPBase::set_part_of(QString part_of)
{
    // optional, no check
    this->part_of = part_of;
}

void PisiSPBase::set_license(QString license) throw(QString)
{
    if(license.isEmpty())
        throw QObject::tr("Empty license !");

    this->license = license;
}

void PisiSPBase::set_is_a(QString is_a)
{
    // zero or more, no check
    this->is_a = is_a;
}

void PisiSPBase::set_build_dependencies(QMap<QString, QMap<PisiSPBase::VRTFAttr, QString> > build_dependencies)
{
    // optional, no check
    this->build_dependencies = build_dependencies;
}

void PisiSPBase::set_build_dependencies(QString build_dependency_string)
{
    QMap<QString, QMap<VRTFAttr, QString> > build_dependencies = get_dependency_list(build_dependency_string);
    set_build_dependencies(build_dependencies);
}

void PisiSPBase::set_aditional_files(QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > aditional_files)
{
    // optional, no check
    this->aditional_files = aditional_files;
}

QString PisiSPBase::get_element_value(QDomElement root, QString tag) throw(QString)
{
    QDomElement elm = root.firstChildElement(tag);
    if(elm.isNull())
    {
        if(is_mandatory(root, tag))
            throw QObject::tr("No %1 tag !").arg(tag);
        else return QString();
    }
    else
    {
//        qDebug() << tag << " : " << elm.text();
        return elm.text();
    }
}

QDomElement PisiSPBase::set_element_value(QDomElement root, QString tag, QString value, QString insert_after) throw(QString)
{
    if(value.isEmpty())
        if(is_mandatory(root, tag))
            throw QObject::tr("%1 tag is mandatory but empty !").arg(tag);
        else
            return QDomElement();

    QDomElement elm = root.firstChildElement(tag);
    if( ! insert_after.isEmpty()){
        if( ! elm.isNull())
            root.removeChild(elm);
        elm = insert_element_after(root, tag, insert_after);
    }
    else{
        if(elm.isNull())
            elm = append_element(root, tag);
    }
    append_text_element(elm, value);
    return elm;
}

QDomElement PisiSPBase::append_element(QDomElement & root, QString tag) throw(QString)
{
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.appendChild(elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomElement PisiSPBase::insert_element_after(QDomElement & root, QString tag, QString after_this_tag) throw(QString)
{
    QDomElement pivot_elm = root.firstChildElement(after_this_tag);
    if(pivot_elm.isNull())
        throw QObject::tr("Trying to insert %1 after %2 but no %2 tag !").arg(tag).arg(after_this_tag);
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.insertAfter(elm,pivot_elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomElement PisiSPBase::insert_element_before(QDomElement & root, QString tag, QString before_this_tag) throw(QString)
{
    QDomElement pivot_elm = root.firstChildElement(before_this_tag);
    if(pivot_elm.isNull())
        throw QObject::tr("Trying to insert %1 before %2 but no %2 tag !").arg(tag).arg(before_this_tag);
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.insertBefore(elm,pivot_elm).isNull())
        throw QObject::tr("Error while creating dom element %1 in %2").arg(tag).arg(root.tagName());
    return elm;
}

QDomText PisiSPBase::append_text_element(QDomElement root, QString value) throw(QString)
{
    QDomText text = root.ownerDocument().createTextNode(value);
    if(text.isNull() || root.appendChild(text).isNull())
        throw QObject::tr("Error creating text element with %2 in to the %2").arg(value).arg(root.tagName());
    return text;
}

QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_dependency(QDomElement elm) throw(QString)
{
    if(elm.isNull())
    {
        return QMap<QString, QMap<VRTFAttr,QString> >();
    }

    QMap<QString, QMap<VRTFAttr,QString> > dependencies;

    elm = elm.firstChildElement("Dependency");
    if(elm.isNull())
        return dependencies;

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Dependency"))
    {
        QMap<VRTFAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull())
                throw QObject::tr("Can not convert to attribute !");
            attributes[get_dependency_attribute(a.name())] = a.value();
//            qDebug() << "DepAttr = " << a.name() << " : " << a.value();
        }
        dependencies[elm.text()] = attributes;
//        qDebug() << "Dep:" << elm.text();
    }
    return dependencies;
}

void PisiSPBase::set_dependency(QDomElement root, QMap<QString, QMap<VRTFAttr,QString> > deps)
{
    if(deps.isEmpty())
    {
        return;
    }

    QList<QString> dependencies = deps.keys();
    for(int i=0; i<dependencies.count(); ++i)
    {
        QString dependency = dependencies.at(i);
        QDomElement elm = append_element(root, "Dependency");
        append_text_element(elm, dependency);

        QMap<VRTFAttr,QString> attr = deps[dependency];
        QList<VRTFAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            VRTFAttr a = attributes.at(j);
            QString a_value = attr[a];
            elm.setAttribute(get_dependency_attribute(a), a_value);
        }
    }
}

PisiSPBase::VRTFAttr PisiSPBase::get_dependency_attribute(QString attr_name, bool abbreviation) throw(QString)
{
    if( ! abbreviation)
    {
        if(attr_name.toLower() == "versionfrom")
            return VERSIONFROM;
        else if(attr_name.toLower() == "versionto")
            return VERSIONTO;
        else if(attr_name.toLower() == "version")
            return VERSION;
        else if(attr_name.toLower() == "releasefrom")
            return RELEASEFROM;
        else if(attr_name.toLower() == "releaseto")
            return RELEASETO;
        else if(attr_name.toLower() == "release")
            return RELEASE;
        else throw QObject::tr("Wrong dependency atribute name : %1").arg(attr_name);
    }
    else
    {
        if(attr_name.startsWith("=="))
            return RELEASE;
        else if(attr_name.startsWith("<<"))
            return RELEASETO;
        else if(attr_name.startsWith(">>"))
            return RELEASEFROM;
        else if(attr_name.startsWith("="))
            return VERSION;
        else if(attr_name.startsWith("<"))
            return VERSIONTO;
        else if(attr_name.startsWith(">"))
            return VERSIONFROM;
        else throw QObject::tr("No VersionReleaseToFrom attribute !");
    }
}

QString PisiSPBase::get_dependency_attribute(PisiSPBase::VRTFAttr attr, bool abbreviation)
{
    switch(attr)
    {
    case VERSIONFROM:
        if(abbreviation) return ">"; else return "versionFrom";
        break;
    case VERSIONTO:
        if(abbreviation) return "<"; else return "versionTo";
        break;
    case VERSION:
        if(abbreviation) return "="; else return "version";
        break;
    case RELEASEFROM:
        if(abbreviation) return ">>"; else return "releaseFrom";
        break;
    case RELEASETO:
        if(abbreviation) return "<<"; else return "releaseTo";
        break;
    case RELEASE:
        if(abbreviation) return "=="; else return "release";
        break;
    default:
        return "";
        break;
    }
}

QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > PisiSPBase::get_aditional_file(QDomElement elm) throw(QString)
{
    if(elm.isNull())
    {
        return QMap<QString, QMap<AFileAttr,QString> >();
    }
    elm = elm.firstChildElement("AditionalFile");

    if(elm.isNull())
    {
        return QMap<QString, QMap<AFileAttr,QString> >();
    }

    QMap<QString, QMap<AFileAttr,QString> > aditional_files;

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("AditionalFile"))
    {
        QMap<AFileAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull())
                throw QObject::tr("Can not convert to attribute !");
            attributes[get_aditional_file_attribute(a.name())] = a.value();
//            qDebug() << "AFileAttr = " << a.name() << " : " << a.value();
        }
        aditional_files[elm.text()] = attributes;
//        qDebug() << "AFile:" << elm.text();
    }
    return aditional_files;
}

void PisiSPBase::set_aditional_file(QDomElement root, QMap<QString, QMap<AFileAttr,QString> > a_files)
{
    if(a_files.isEmpty())
    {
        return;
    }

    QList<QString> aditional_files = a_files.keys();
    for(int i=0; i<aditional_files.count(); ++i)
    {
        QString aditional_file = aditional_files.at(i);
        QDomElement elm = append_element(root, "AditionalFile");
        append_text_element(elm, aditional_file);

        QMap<AFileAttr,QString> attr = a_files[aditional_file];
        QList<AFileAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            AFileAttr a = attributes.at(j);
            QString a_value = attr[a];
            elm.setAttribute(get_aditional_file_attribute(a), a_value);
        }
    }
}

PisiSPBase::AFileAttr PisiSPBase::get_aditional_file_attribute(QString attr_name) throw(QString)
{
        if(attr_name.toLower() == "target")
            return TARGET;
        else if(attr_name.toLower() == "permission")
            return PERMISSION;
        else if(attr_name.toLower() == "owner")
            return OWNER;
        else if(attr_name.toLower() == "group")
            return GROUP;
        else
            throw QObject::tr("Wrong aditional_files atribute name : %1").arg(attr_name);
}

QString PisiSPBase::get_aditional_file_attribute(PisiSPBase::AFileAttr attr)
{
    switch(attr)
    {
    case TARGET:
        return "target";
        break;
    case PERMISSION:
        return "permission";
        break;
    case OWNER:
        return "owner";
        break;
    case GROUP:
        return "group";
        break;
    default:
        return "";
        break;
    }
}

/**
  returns dependencies like : qt[>4.7;<4.5;==4.6], gtk[>>2], libz, libusb1[=1]
*/
QStringList PisiSPBase::get_dependency_list(QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > dependencies)
{
    QStringList dep_list;
    QMap<QString, QMap<VRTFAttr,QString> >::const_iterator dependency_it = dependencies.constBegin();
    for( ; dependency_it != dependencies.constEnd(); dependency_it++)
    {
        QStringList attr_list;
        QMap<VRTFAttr,QString> attr = dependency_it.value();
        QMap<VRTFAttr,QString>::const_iterator attr_it = attr.constBegin();
        for( ; attr_it != attr.constEnd(); attr_it++)
        {
            attr_list << QString("%1%2").arg(get_dependency_attribute(attr_it.key(), true)).arg(attr_it.value());
        }
        QString attributes = ((attr_list.count()>0)?QString("%1%2%3").arg("[").arg(attr_list.join(";")).arg("]"):"");
        dep_list << QString("%1%2").arg(dependency_it.key()).arg(attributes);
//        qDebug() << "dep list : " << dep_list;
    }
    return dep_list;
}

/**
  takes string like : qt[>4.7;<4.5;==4.6], gtk[>>2], libz, libusb1[=1], returns dependencies
*/
QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiSPBase::get_dependency_list(QString dependency_string)
{
    QMap<QString, QMap<VRTFAttr,QString> > dependencies;
    QStringList dependency_list = dependency_string.split(',', QString::SkipEmptyParts);
    if(dependency_list.count() < 1)
        return dependencies;
    for(int i=0; i<dependencies.count(); ++i)
    {
        QString dep = dependency_list.at(i).trimmed();
        if( ! dep.isEmpty())
        {
            QString dep_name;
            QString dep_attr;
            int start_index = dep.indexOf('[');
            int end_index = dep.indexOf(']');
            if(start_index > 0)
            {
                dep_name = dep.left(start_index);
                if(end_index > 0)
                    dep_attr = dep.mid(start_index);
            }
            else
                dep_name = dep;
            dependencies[dep_name] = get_dependency_attr_list(dep_attr);
        }
    }
    return dependencies;
}

/**
  helper function to return dependency attributes, takes strin like : >4.7;<4.5;==4.6
*/

QMap<PisiSPBase::VRTFAttr,QString> PisiSPBase::get_dependency_attr_list(QString attr_string)
{
    QMap<VRTFAttr,QString> attributes;
    QStringList attr_list = attr_string.split(';', QString::SkipEmptyParts);
    if(attr_list.count() < 1)
        return attributes;
    for(int i=0; i<attr_list.count(); ++i)
    {
        QString attr = attr_list.at(i).trimmed();
        if( ! attr.isEmpty())
        {
            VRTFAttr a = get_dependency_attribute(attr, true);
            QString t = get_dependency_attribute(a, true);
            int start_index = attr.indexOf(t) + t.length();
            attributes[a] = t.mid(start_index);
        }
    }
    return attributes;
}


bool PisiSPBase::operator ==(const PisiSPBase & other) const
{
    return (
                get_name()                  == other.get_name()
                && get_summary()            == other.get_summary()
                && get_description()        == other.get_description()
                && get_part_of()            == other.get_part_of()
                && get_license()            == other.get_license()
                && get_is_a()               == other.get_is_a()
                && get_build_dependencies() == other.get_build_dependencies()
                && get_aditional_files()    == other.get_aditional_files()
                );
}

bool PisiSPBase::operator !=(const PisiSPBase & other) const
{
    return ! (*this == other);
}
