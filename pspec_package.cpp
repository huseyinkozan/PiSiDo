#include "pspec_package.h"

#include <QDomDocumentFragment>

PSpecPackage::PSpecPackage()
    : PSpecBase()
{
}


void PSpecPackage::clear()
{
    PSpecBase::clear();
    runtime_dependencies.clear();
}

bool PSpecPackage::load_from_dom(const QDomDocumentFragment & dom_fragment)
{
    if(dom_fragment.isNull())
        return false;

    PSpecBase::load_from_dom(dom_fragment);

    // TODO : implement

    return true;
}
