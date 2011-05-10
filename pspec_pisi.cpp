#include "pspec_pisi.h"

#include <QDomDocument>

PSpecPISI::PSpecPISI()
{
}


void PSpecPISI::clear()
{
    source.clear();
    package.clear();
    updates.clear();
}

bool PSpecPISI::load_from_dom(const QDomDocument & dom)
{
    QDomElement root = dom.documentElement();
    if( ! root.isNull() && root.tagName().toLower() == "pisi")
    {
        clear();

        QDomElement elm_src = root.namedItem("Source").toElement();
        if( ! elm_src.isNull() && elm_src.isElement())
        {
            if( ! source.load_from_dom(elm_src.toDocumentFragment()))
                return false;
        }
        else
            return false;

        QDomElement elm_pkg = root.namedItem("Package").toElement();
        if( ! elm_pkg.isNull() && elm_pkg.isElement())
        {
            if( ! source.load_from_dom(elm_pkg.toDocumentFragment()))
                return false;
        }
        else
            return false;

        QDomElement elm_hist = root.namedItem("History").toElement();
        if( ! elm_hist.isNull() && elm_hist.isElement())
        {
            QDomElement elm_upd = elm_hist.firstChildElement("Update");
            for( ; ! elm_upd.isNull(); elm_upd = elm_upd.nextSiblingElement("Update"))
            {
                PSpecUpdate upd;
                if(upd.load_from_dom(elm_upd.toDocumentFragment()))
                    updates.append(upd);
                else
                    return false;
            }
        }
        else
            return false;
    }
    return false;
}

bool PSpecPISI::save_to_dom(QDomDocument &dom)
{
    if(dom.isNull())
        return false;

    // TODO : implement

    return true;
}
