#include "pisisource.h"

#include <QDebug>
#include <QDomElement>

PisiSource::PisiSource()
    : PisiSPBase()
{
    clear();
}

void PisiSource::clear()
{
    PisiSPBase::clear();
    home_page.clear();
    packager.clear();
    archives.clear();
}

void PisiSource::load_from_dom(const QDomElement & dom_element)
{
    PisiSPBase::load_from_dom(dom_element);

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

QString PisiSource::get_home_page() const
{
    return home_page;
}

QMap<QString,QString> PisiSource::get_packager() const
{
    return packager;
}

QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > PisiSource::get_archives() const
{
    return archives;
}

void PisiSource::set_home_page(QString home_page)
{
    if(home_page.isEmpty())
        throw QString("Homepage can not be empty !");

    this->home_page = home_page;
}

void PisiSource::set_packager(QMap<QString, QString> packager)
{
    if(packager.isEmpty())
        throw QString("Empty packager map !");
    if(packager.keys().first().isEmpty())
        throw QString("Empty packager name !");
    if(packager.values().first().isEmpty())
        throw QString("Empty packager email !");

    this->packager = packager;
}

void PisiSource::set_archives(QMap<QString, QMap<PisiSource::ArchiveAttr, QString> > archives)
{
    if(archives.count() == 0)
        throw QString("Empty archive !");

    this->archives = archives;
}

PisiSource::ArchiveAttr PisiSource::get_archive_attr_property(QString attr_name)
{
    if(attr_name.toLower() == "sha1sum")
        return SHA1SUM;
    else if(attr_name.toLower() == "type")
        return TYPE;
    else if(attr_name.toLower() == "target")
        return TARGET;
    else throw QString("Wrong archive atribute name : %1").arg(attr_name);
}

bool PisiSource::operator ==(const PisiSource & other)
{
    return (
                PisiSPBase::operator ==(other)
                && get_home_page() == other.get_home_page()
                && get_packager() == other.get_packager()
                && get_archives() == other.get_archives()
                );
}

bool PisiSource::operator !=(const PisiSource & other)
{
    return ! (*this == other);
}
