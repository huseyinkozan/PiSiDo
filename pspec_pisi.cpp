#include "pspec_pisi.h"

#include <QDomDocument>

PSpecPISI::PSpecPISI()
{
    clear();
}


void PSpecPISI::clear()
{
    loaded = false;
    source.clear();
    package.clear();
    updates.clear();
}

PSpecSource PSpecPISI::get_source()
{
    if(is_loaded())
        return source;
    else
        return PSpecSource();
}

PSpecPackage PSpecPISI::get_package()
{
    if(is_loaded())
        return package;
    else
        return PSpecPackage();
}

QList<PSpecUpdate> PSpecPISI::get_updates()
{
    if(is_loaded())
        return updates;
    else
        return QList<PSpecUpdate>();
}

void PSpecPISI::load_from_dom(const QDomDocument & dom)
{
    loaded = false;

    QDomElement root = dom.documentElement();
    if( ! root.isNull() && root.tagName().toLower() == "pisi")
    {
        clear();

        QDomElement elm_src = root.namedItem("Source").toElement();
        if( ! elm_src.isNull() && elm_src.isElement())
        {
            try {
                source.load_from_dom(elm_src);
            } catch (QString e) {
                throw QString("From Source parser : %1").arg(e);
            }
        }
        else
        {
            throw QString("Can not find Source tag !");
        }

        QDomElement elm_pkg = root.namedItem("Package").toElement();
        if( ! elm_pkg.isNull() && elm_pkg.isElement())
        {
            try{
                package.load_from_dom(elm_pkg);
            } catch (QString e) {
                throw QString("From Package parser : %1").arg(e);
            }
        }
        else
        {
            throw QString("Can not find Package tag !");
        }

        QDomElement elm_hist = root.namedItem("History").toElement();
        if( ! elm_hist.isNull() && elm_hist.isElement())
        {
            QDomElement elm_upd = elm_hist.firstChildElement("Update");
            for( ; ! elm_upd.isNull(); elm_upd = elm_upd.nextSiblingElement("Update"))
            {
                PSpecUpdate upd;
                try {
                    upd.load_from_dom(elm_upd);
                } catch (QString e) {
                    throw QString("From Update parser : %1").arg(e);
                }
                updates.append(upd);
            }
        }
        else
        {
            throw QString("Can not find History tag !");
        }

        loaded = true;
    }
    else
    {
        throw QString("Can not find PISI tag !");
    }
}

bool PSpecPISI::save_to_dom(QDomDocument &dom)
{
    if(dom.isNull())
        return false;

    // TODO : implement

    return true;
}
