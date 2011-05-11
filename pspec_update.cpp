#include "pspec_update.h"

#include <QDomElement>

PSpecUpdate::PSpecUpdate()
{
    clear();
}

void PSpecUpdate::clear()
{
    date = QDate();
    version.clear();
    comment.clear();
    packager_name.clear();
    packager_email.clear();
    release = 0;
}

void PSpecUpdate::load_from_dom(const QDomElement & dom_element)
{
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecPackage !");

    QDomElement elm = dom_element.firstChildElement("Version");
    version = get_value_from_element("Version", elm, true);

    elm = dom_element.firstChildElement("Comment");
    comment = get_value_from_element("Comment", elm, true);

    elm = dom_element.firstChildElement("Name");
    packager_name = get_value_from_element("Name", elm, true);

    elm = dom_element.firstChildElement("Email");
    packager_email = get_value_from_element("Email", elm, true);

    elm = dom_element.firstChildElement("Date");
    date = QDate::fromString(get_value_from_element("Date", elm, true),"yyyy-MM-dd");

    bool ok = false;
    int v = dom_element.attribute("release", "0").toInt(&ok);
    if(ok) release = v;
    else release = 0;
}

QString PSpecUpdate::get_value_from_element(QString tag, QDomElement elm, bool mandatory)
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
