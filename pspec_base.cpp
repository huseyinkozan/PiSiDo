#include "pspec_base.h"

#include <QDomElement>

#include <QTextStream>
#include <QDebug>

PSpecBase::PSpecBase()
{
}

void PSpecBase::clear()
{
    name.clear();
    summary.clear();
    description.clear();
    part_of.clear();
    license.clear();
    is_a.clear();
    build_dependencies.clear();
}

void PSpecBase::load_from_dom(const QDomElement & dom_element)
{
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecBase !");

    bool is_src_tag = dom_element.tagName().toLower() == "source";

    QDomElement elm = dom_element.firstChildElement("Name");
    if(elm.isNull()) throw QString("No child element in package or source !");

//    qDebug() << (is_src_tag?"Source":"Package") << ":: Base";

    name = get_value_from_element("Name", elm, true);

    elm = elm.nextSiblingElement("License");
    license = get_value_from_element("License", elm, is_src_tag);

    elm = elm.nextSiblingElement("Summary");
    summary = get_value_from_element("Summary", elm, is_src_tag);

    elm = elm.nextSiblingElement("Description");
    description = get_value_from_element("Description", elm, false);

    elm = elm.nextSiblingElement("PartOf");
    part_of = get_value_from_element("PartOf", elm, false);

    elm = elm.nextSiblingElement("IsA");
    is_a = get_value_from_element("IsA", elm, false);

    elm = dom_element.firstChildElement("BuildDependencies");
    if( ! elm.isNull())
    {
        elm = elm.firstChildElement("Dependency");
        if(elm.isNull())
            throw QString("No Dependency in Dependency");

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
//                qDebug() << "BuildDepAttr = " << a.name() << " : " << a.value();
            }
            build_dependencies[elm.text()] = attributes;
        }
    }
}

QString PSpecBase::get_value_from_element(QString tag, QDomElement elm, bool mandatory)
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

PSpecBase::VersionReleaseToFromAttr PSpecBase::get_dependency_attr_property(QString attr_name)
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
