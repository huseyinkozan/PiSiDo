#include "pisi.h"

#include <QDomDocument>

Pisi::Pisi()
{
    clear();
}


void Pisi::clear()
{
    empty = true;
    source.clear();
    package.clear();
    updates.clear();
    last_update.clear();
}

bool Pisi::is_empty() const
{
    return empty;
}

PisiSource Pisi::get_source() const
{
    if(is_empty())
        return PisiSource();
    else
        return source;
}

PisiPackage Pisi::get_package() const
{
    if(is_empty())
        return PisiPackage();
    else
        return package;
}

QMap<int, PisiUpdate> Pisi::get_updates() const
{
    if(is_empty())
        return QMap<int, PisiUpdate>();
    else
        return updates;
}

PisiUpdate Pisi::get_last_update() const
{
    return last_update;
}

void Pisi::set_source(PisiSource source)
{
    if(source == PisiSource())
        throw QString("Empty source class !");

    this->source = source;
    empty = false;
}

void Pisi::set_package(PisiPackage package)
{
    if(package == PisiPackage())
        throw QString("Empty package class !");

    this->package = package;
    empty = false;
}

void Pisi::set_updates(QMap<int, PisiUpdate> updates)
{
    if(updates.isEmpty())
        throw QString("Empty history update !");

    this->updates = updates;
    empty = false;
}

void Pisi::load_from_dom(const QDomDocument & dom)
{
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
            int last_release = 0;
            QDomElement elm_upd = elm_hist.firstChildElement("Update");
            for( ; ! elm_upd.isNull(); elm_upd = elm_upd.nextSiblingElement("Update"))
            {
                PisiUpdate upd;
                try {
                    upd.load_from_dom(elm_upd);
                } catch (QString e) {
                    throw QString("From Update parser : %1").arg(e);
                }
                int release = upd.get_release();
                if(release > last_release)
                    last_update = upd;
                updates[release] = upd;
            }
        }
        else
        {
            throw QString("Can not find History tag !");
        }

        empty = false;
    }
    else
    {
        throw QString("Can not find PISI tag !");
    }
}

bool Pisi::save_to_dom(QDomDocument &dom)
{
    if(dom.isNull())
        throw QString("Empty dom while saving pisi class values to dom !");
    if(is_empty())
        throw QString("Empty pisi class while saving pisi class values to dom !");

    QDomElement root = dom.documentElement();
    if( ! root.isNull() && root.tagName().toLower() == "pisi")
    {
        QDomElement elm_src = root.namedItem("Source").toElement();
        if(elm_src.isNull() || ! elm_src.isElement())
        {
            elm_src = root.ownerDocument().createElement("Source");
            root.appendChild(elm_src);
        }
        try {
            source.save_to_dom(elm_src);
        } catch (QString e) {
            throw QString("From Source saver : %1").arg(e);
        }

        QDomElement elm_pkg = root.namedItem("Package").toElement();
        if(elm_pkg.isNull() || ! elm_pkg.isElement())
        {
            elm_pkg = root.ownerDocument().createElement("Package");
            root.appendChild(elm_pkg);
        }
        try {
            package.save_to_dom(elm_pkg);
        } catch (QString e) {
            throw QString("From Package saver : %1").arg(e);
        }

        QDomElement elm_hist = root.namedItem("History").toElement();
        if(elm_hist.isNull() || ! elm_hist.isElement())
        {
            elm_hist = root.ownerDocument().createElement("History");
            root.appendChild(elm_hist);
        } else
            if(elm_hist.hasChildNodes())
            {
                QDomNodeList nodes = elm_hist.childNodes();
                for(int i=0; i<nodes.count(); ++i)
                {
                    elm_hist.removeChild(nodes.at(i));
                }
            }

        QList<int> releases = updates.keys();
        for(int i=releases.count()-1; i>=0; --i)
        {
            QDomElement elm_upd = root.ownerDocument().createElement("Update");
            elm_hist.appendChild(elm_upd);
            try {
                updates[releases.at(i)].save_to_dom(elm_upd);
            } catch (QString e) {
                throw QString("From Update saver : %1").arg(e);
            }
        }
    }
    else
    {
        throw QString("Can not find PISI tag !");
    }

    return true;
}
