#include "pspec_update.h"

#include <QDomElement>

PSpecUpdate::PSpecUpdate()
{
}

void PSpecUpdate::clear()
{
    date = QDate();
    version.clear();
    comment.clear();
    packager_name.clear();
    packager_email.clear();
}

void PSpecUpdate::load_from_dom(const QDomElement & dom_element)
{
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecPackage !");

    // TODO : implement
}
