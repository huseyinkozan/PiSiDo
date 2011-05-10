#include "pspec_update.h"

#include <QDomDocumentFragment>

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

bool PSpecUpdate::load_from_dom(const QDomDocumentFragment & dom_fragment)
{
    if(dom_fragment.isNull())
        return false;

    // TODO : implement

    return true;
}
