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

void PisiSPBase::load_from_dom(const QDomElement & dom_element)
{
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecBase !");

    bool is_src_tag = dom_element.tagName().toLower() == "source";
    //    qDebug() << (is_src_tag?"Source":"Package") << ":: Base";

    QDomElement elm = dom_element.firstChildElement("Name");
    name = get_value_from_element("Name", elm, true);

    elm = dom_element.firstChildElement("License");
    license = get_value_from_element("License", elm, is_src_tag);

    elm = dom_element.firstChildElement("Summary");
    summary = get_value_from_element("Summary", elm, is_src_tag);

    elm = dom_element.firstChildElement("Description");
    description = get_value_from_element("Description", elm, false);

    elm = dom_element.firstChildElement("PartOf");
    part_of = get_value_from_element("PartOf", elm, false);

    elm = dom_element.firstChildElement("IsA");
    is_a = get_value_from_element("IsA", elm, false);

    elm = dom_element.firstChildElement("BuildDependencies");
    build_dependencies = get_dependency_map(elm, false);
}

QString PisiSPBase::get_name()
{
    return name;
}

QString PisiSPBase::get_summary()
{
    return summary;
}

QString PisiSPBase::get_description()
{
    return description;
}

QString PisiSPBase::get_part_of()
{
    return part_of;
}

QString PisiSPBase::get_license()
{
    return license;
}

QString PisiSPBase::get_is_a()
{
    return is_a;
}

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiSPBase::get_build_dependencies()
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

void PisiSPBase::set_build_dependencies(QMap<QString, QMap<VersionReleaseToFromAttr, QString> > build_dependencies)
{
    // optional, no check
    this->build_dependencies = build_dependencies;
}

QString PisiSPBase::get_value_from_element(QString tag, QDomElement elm, bool mandatory)
{
    if(elm.isNull())
    {
        if(mandatory)
            throw QString("No %1 tag !").arg(tag);
        else return QString();
    }
    else
    {
//        qDebug() << tag << " : " << elm.text();
        return elm.text();
    }
}

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiSPBase::get_dependency_map(QDomElement elm, bool mandatory)
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

PisiSPBase::VersionReleaseToFromAttr PisiSPBase::get_dependency_attr_property(QString attr_name)
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

/*
  qt[>4.7,<4.5,==4.6], gtk[>>2], libz, libusb1[=1]
*/
QStringList PisiSPBase::get_dependency_list(QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > dependency)
{
    QStringList dep_list;
    QMap<QString, QMap<VersionReleaseToFromAttr,QString> >::const_iterator dependency_it = dependency.constBegin();
    for( ; dependency_it != dependency.constEnd(); dependency_it++)
    {
        QStringList attr_list;
        QMap<VersionReleaseToFromAttr,QString> attr = dependency_it.value();
        QMap<VersionReleaseToFromAttr,QString>::const_iterator attr_it = attr.constBegin();
        for( ; attr_it != attr.constEnd(); attr_it++)
        {
            attr_list << QString("%1%2").arg(get_dependency_attr_property_string(attr_it.key(), true)).arg(attr_it.value());
        }
        QString attributes = ((attr_list.count()>0)?QString("%1%2%3").arg("[").arg(attr_list.join(",")).arg("]"):"");
        dep_list << QString("%1%2").arg(dependency_it.key()).arg(attributes);
//        qDebug() << "dep list : " << dep_list;
    }
    return dep_list;
}
