#include "pspec_source.h"

#include <QDomElement>

PSpecSource::PSpecSource()
    : PSpecBase()
{
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
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecSource !");

    PSpecBase::load_from_dom(dom_element);

    // TODO : implement
}
