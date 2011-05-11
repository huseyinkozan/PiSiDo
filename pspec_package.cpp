#include "pspec_package.h"

#include <QDomElement>

PSpecPackage::PSpecPackage()
    : PSpecBase()
{
}


void PSpecPackage::clear()
{
    PSpecBase::clear();
    runtime_dependencies.clear();
}

void PSpecPackage::load_from_dom(const QDomElement & dom_element)
{
    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecPackage !");

    PSpecBase::load_from_dom(dom_element);

    // TODO : implement
}
