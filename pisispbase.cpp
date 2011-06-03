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
}

void PisiSPBase::load_from_dom(const QDomElement & root_elm)
{
    if(root_elm.isNull())
        throw QString("Dom Element is null while loading to PisiSPBase !");


    name = get_value_from_element("Name", root_elm);
    license = get_value_from_element("License", root_elm);
    summary = get_value_from_element("Summary", root_elm);
    description = get_value_from_element("Description", root_elm);
    part_of = get_value_from_element("PartOf", root_elm);
    is_a = get_value_from_element("IsA", root_elm);

    QDomElement elm = root_elm.firstChildElement("BuildDependencies");
    build_dependencies = get_dep_from_element(elm, is_mandatory(root_elm, "BuildDependencies"));
}

void PisiSPBase::save_to_dom(QDomElement & root_elm)
{
    if(root_elm.isNull())
        throw QString("Dom Element is null while saving from PisiSPBase to dom !");

    set_value_to_element(name, "Name", root_elm);
    set_value_to_element(license, "License", root_elm);
    set_value_to_element(summary, "Summary", root_elm);
    set_value_to_element(description, "Description", root_elm);
    set_value_to_element(part_of, "PartOf", root_elm);
    set_value_to_element(is_a, "IsA", root_elm);

    QDomElement elm = root_elm.firstChildElement("BuildDependencies");
    if( ! elm.isNull())
        root_elm.removeChild(elm);
    elm = get_appended_dom_elm(root_elm, "BuildDependencies");
    set_dep_to_element(build_dependencies, elm, is_mandatory(root_elm, "BuildDependencies"));
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

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiSPBase::get_build_dependencies() const
{
    return build_dependencies;
}

void PisiSPBase::set_name(QString name)
{
    if(name.isEmpty())
        throw QString("Empty name !");

    this->name = name;
}

void PisiSPBase::set_summary(QString summary)
{
    if(summary.isEmpty())
        throw QString("Empty summary !");

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

void PisiSPBase::set_license(QString license)
{
    if(license.isEmpty())
        throw QString("Empty license !");

    this->license = license;
}

void PisiSPBase::set_is_a(QString is_a)
{
    // zero or more, no check
    this->is_a = is_a;
}

void PisiSPBase::set_build_dependencies(QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr, QString> > build_dependencies)
{
    // optional, no check
    this->build_dependencies = build_dependencies;
}

void PisiSPBase::set_build_dependencies(QString build_dependency_string)
{
    QMap<QString, QMap<VersionReleaseToFromAttr, QString> > build_dependencies = get_dependency_list(build_dependency_string);
    set_build_dependencies(build_dependencies);
}

QString PisiSPBase::get_value_from_element(QString tag, QDomElement root)
{
    QDomElement elm = root.firstChildElement("Name");
    if(elm.isNull())
    {
        if(is_mandatory(root, tag))
            throw QString("No %1 tag !").arg(tag);
        else return QString();
    }
    else
    {
//        qDebug() << tag << " : " << elm.text();
        return elm.text();
    }
}

void PisiSPBase::set_value_to_element(QString value, QString tag, QDomElement root)
{
    QDomElement elm = root.firstChildElement(tag);
    if(elm.isNull())
        elm = get_appended_dom_elm(root, tag);

    if(is_mandatory(root, tag) && value.isEmpty())
        throw QString("%1 tag is mandatory but empty !");
    appended_dom_text(elm, value);
}

QDomElement PisiSPBase::get_appended_dom_elm(QDomElement & root, QString tag)
{
    QDomElement elm = root.ownerDocument().createElement(tag);
    if(elm.isNull() || root.appendChild(elm).isNull())
        throw QString("Error while creating dom element %1").arg(tag);
    return elm;
}

QDomText PisiSPBase::appended_dom_text(QDomElement elm, QString value)
{
    QDomText text = elm.ownerDocument().createTextNode(value);
    if(text.isNull() || elm.appendChild(text).isNull())
        throw QString("Error while creating dom text element with value = %2").arg(value);
}

bool PisiSPBase::is_mandatory(QDomElement root, QString tag)
{
    bool is_src;
    if(root.tagName().toLower() == "source")
        is_src = true;
    else if(root.tagName().toLower() == "package")
        is_src = false;
    else
        throw QString("Wrong root dom passed to is_mandatory() !");

    if(tag == "Name")
        return true;
    else if(tag == "License")
        return is_src;
    else if(tag == "Summary")
        return is_src;
    else if(tag == "Description")
        return false;
    else if(tag == "PartOf")
        return false;
    else if(tag == "IsA")
        return false;
    else if(tag == "BuildDependencies")
        return false;
    else
        throw QString("Undefined tag name in is_mandatory() !");
}

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiSPBase::get_dep_from_element(QDomElement elm, bool mandatory)
{
    if(elm.isNull())
    {
        if(mandatory)
            throw QString("Empty element while getting dependency map.");
        else return QMap<QString, QMap<VersionReleaseToFromAttr,QString> >();
    }

    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > dependencies;

    elm = elm.firstChildElement("Dependency");
    if(elm.isNull())
        throw QString("No Dependency in Dependencies");

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Dependency"))
    {
        QMap<VersionReleaseToFromAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull())
                throw QString("Can not convert to attribute !");
            attributes[get_dependency_attr_property(a.name())] = a.value();
//            qDebug() << "DepAttr = " << a.name() << " : " << a.value();
        }
        dependencies[elm.text()] = attributes;
//        qDebug() << "Dep:" << elm.text();
    }
    return dependencies;
}

void PisiSPBase::set_dep_to_element(QMap<QString, QMap<VersionReleaseToFromAttr,QString> > dep, QDomElement elm, bool mandatory)
{
    if(dep.isEmpty())
    {
        if(mandatory)
            throw QString("Empty dependency while setting dependency tag.");
        else
            return;
    }

    QList<QString> dependencies = dep.keys();
    for(int i=0; i<dependencies.count(); ++i)
    {
        QString dependency = dependencies.at(i);
        QDomElement elm = get_appended_dom_elm(elm, "Dependency");
        appended_dom_text(elm, dependency);

        QMap<VersionReleaseToFromAttr,QString> attr = dep[dependency];
        QList<VersionReleaseToFromAttr> attributes = attr.keys();
        for(int j=0; j<attributes.count(); ++j)
        {
            VersionReleaseToFromAttr v = attributes.at(j);
            QString release = attr[v];
            elm.setAttribute(get_dependency_attr_property_string(v), release);
        }
    }
}

PisiSPBase::VersionReleaseToFromAttr PisiSPBase::get_dependency_attr_property(QString attr_name, bool abbreviation)
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
        else throw QString("Wrong dependency atribute name : %1").arg(attr_name);
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
        else throw QString("No VersionReleaseToFrom attribute !");
    }
}

QString PisiSPBase::get_dependency_attr_property_string(PisiSPBase::VersionReleaseToFromAttr attr, bool abbreviation)
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

/**
  returns dependencies like : qt[>4.7;<4.5;==4.6], gtk[>>2], libz, libusb1[=1]
*/
QStringList PisiSPBase::get_dependency_list(QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > dependencies)
{
    QStringList dep_list;
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> >::const_iterator dependency_it = dependencies.constBegin();
    for( ; dependency_it != dependencies.constEnd(); dependency_it++)
    {
        QStringList attr_list;
        QMap<VersionReleaseToFromAttr,QString> attr = dependency_it.value();
        QMap<VersionReleaseToFromAttr,QString>::const_iterator attr_it = attr.constBegin();
        for( ; attr_it != attr.constEnd(); attr_it++)
        {
            attr_list << QString("%1%2").arg(get_dependency_attr_property_string(attr_it.key(), true)).arg(attr_it.value());
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
QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiSPBase::get_dependency_list(QString dependency_string)
{
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> > dependencies;
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

QMap<PisiSPBase::VersionReleaseToFromAttr,QString> PisiSPBase::get_dependency_attr_list(QString attr_string)
{
    QMap<VersionReleaseToFromAttr,QString> attributes;
    QStringList attr_list = attr_string.split(';', QString::SkipEmptyParts);
    if(attr_list.count() < 1)
        return attributes;
    for(int i=0; i<attr_list.count(); ++i)
    {
        QString attr = attr_list.at(i).trimmed();
        if( ! attr.isEmpty())
        {
            VersionReleaseToFromAttr a = get_dependency_attr_property(attr, true);
            QString t = get_dependency_attr_property_string(a, true);
            int start_index = attr.indexOf(t) + t.length();
            attributes[a] = t.mid(start_index);
        }
    }
    return attributes;
}


bool PisiSPBase::operator ==(const PisiSPBase & other) const
{
    return (
                get_name() == other.get_name()
                && get_summary() == other.get_summary()
                && get_description() == other.get_description()
                && get_part_of() == other.get_part_of()
                && get_license() == other.get_license()
                && get_is_a() == other.get_is_a()
                && get_build_dependencies() == other.get_build_dependencies()
                );
}

bool PisiSPBase::operator !=(const PisiSPBase & other) const
{
    return ! (*this == other);
}
