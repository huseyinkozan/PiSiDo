#include "pspec_source.h"

#include <QDomDocumentFragment>

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

bool PSpecSource::load_from_dom(const QDomDocumentFragment & dom_fragment)
{
    if(dom_fragment.isNull())
        return false;

    PSpecBase::load_from_dom(dom_fragment);

    // TODO : implement

    return true;
}
