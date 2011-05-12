#include "pspec_source.h"

#include <QDebug>
#include <QDomElement>

PSpecSource::PSpecSource()
    : PSpecBase()
{
    clear();
}

void PSpecSource::clear()
{
    PSpecBase::clear();
    home_page.clear();
    packager.clear();
    archives.clear();
}

void PSpecSource::load_from_dom(const QDomElement & dom_element)
{
    PSpecBase::load_from_dom(dom_element);

    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecSource !");

    QDomElement elm = dom_element.firstChildElement("Homepage");
    if(elm.isNull()) throw QString("No Homepage tag !");
    home_page = elm.text();

    elm = dom_element.firstChildElement("Packager");
    if(elm.isNull()) throw QString("No Packager tag !");

    elm = elm.firstChildElement("Name");
    if(elm.isNull()) throw QString("No Packager::Name tag !");
    QString packager_name = elm.text();

    elm = elm.nextSiblingElement("Email");
    if(elm.isNull()) throw QString("No Packager::Email tag !");
    packager[packager_name] = elm.text();

    elm = dom_element.firstChildElement("Archive");
    if(elm.isNull()) throw QString("No Archive tag !");

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Archive"))
    {
        QMap<ArchiveAttr,QString> attributes;
        QDomNamedNodeMap elm_node_map = elm.attributes();
        int count = elm_node_map.count();
        for(int i=0; i<count; ++i)
        {
            QDomNode n = elm_node_map.item(i);
            QDomAttr a = n.toAttr();
            if(a.isNull()) throw QString("Can not convert to attribute !");
            attributes[get_archive_attr_property(a.name())] = a.value();
//                qDebug() << "ArchiveAttr = " << a.name() << " : " << a.value();
        }
        archives[elm.text()] = attributes;
    }
}

QString PSpecSource::get_home_page()
{
    return home_page;
}

QMap<QString,QString> PSpecSource::get_packager()
{
    return packager;
}

QMap<QString, QMap<PSpecSource::ArchiveAttr,QString> > PSpecSource::get_archives()
{
    return archives;
}

PSpecSource::ArchiveAttr PSpecSource::get_archive_attr_property(QString attr_name)
{
    if(attr_name.toLower() == "sha1sum")
        return SHA1SUM;
    else if(attr_name.toLower() == "type")
        return TYPE;
    else if(attr_name.toLower() == "target")
        return TARGET;
    else throw QString("Wrong archive atribute name : %1").arg(attr_name);
}
