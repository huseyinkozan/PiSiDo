#include "pspec_base.h"

#include <QDomDocumentFragment>

PSpecBase::PSpecBase()
{
}

void PSpecBase::clear()
{
    name.clear();
    license.clear();
    part_of.clear();
    is_a_s.clear();
    summaries.clear();
    descriptions.clear();
    build_dependencies.clear();
    additional_files.clear();
}

bool PSpecBase::load_from_dom(const QDomDocumentFragment & dom_fragment)
{
    if(dom_fragment.isNull())
        return false;

    return true;
}
