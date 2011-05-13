#include "pisiupdate.h"

#include <QDomElement>

PisiUpdate::PisiUpdate()
{
    clear();
}

void PisiUpdate::clear()
{
    date = QDate();
    version.clear();
    comment.clear();
    packager_name.clear();
    packager_email.clear();
    release = 0;
}

void PisiUpdate::load_from_dom(const QDomElement & dom_element)
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

QString PisiUpdate::get_value_from_element(QString tag, QDomElement elm, bool mandatory)
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

int PisiUpdate::get_release()
{
    return release;
}

QDate PisiUpdate::get_date()
{
    return date;
}

QString PisiUpdate::get_version()
{
    return version;
}

QString PisiUpdate::get_comment()
{
    return comment;
}

QString PisiUpdate::get_packager_name()
{
    return packager_name;
}

QString PisiUpdate::get_packager_email()
{
    return packager_email;
}


void PisiUpdate::set_release(int r)
{
    if(r>0) release = r;
}

void PisiUpdate::set_date(QDate d)
{
    if(d.isValid()) date = d;
}

void PisiUpdate::set_version(QString v)
{
    version = v;
}

void PisiUpdate::set_comment(QString c)
{
    comment = c;
}

void PisiUpdate::set_packager_name(QString p_n)
{
    packager_name = p_n;
}

void PisiUpdate::set_packager_email(QString p_e)
{
    packager_email = p_e;
}
