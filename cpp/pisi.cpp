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

void Pisi::set_source(PisiSource source) throw(QString)
{
    if(source == PisiSource())
        throw QObject::tr("Empty source class !");

    this->source = source;
    empty = false;
}

void Pisi::set_package(PisiPackage package) throw(QString)
{
    if(package == PisiPackage())
        throw QObject::tr("Empty package class !");

    this->package = package;
    empty = false;
}

void Pisi::set_updates(QMap<int, PisiUpdate> updates) throw(QString)
{
    if(updates.isEmpty())
        throw QObject::tr("Empty history update !");

    this->updates = updates;
    empty = false;
    QList<int> release_list = updates.keys();
    if(release_list.count() > 0){
        last_update = updates.value(release_list.last());
    }
}

void Pisi::load_from_dom(const QDomDocument & dom) throw(QString)
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
                throw QObject::tr("From Source parser : %1").arg(e);
            }
        }
        else
        {
            throw QObject::tr("Can not find Source tag !");
        }

        QDomElement elm_pkg = root.namedItem("Package").toElement();
        if( ! elm_pkg.nextSiblingElement("Package").isNull()){
            throw QObject::tr("Multiple Package tag is not supported !");
        }
        if( ! elm_pkg.isNull() && elm_pkg.isElement())
        {
            try{
                package.load_from_dom(elm_pkg);
            } catch (QString e) {
                throw QObject::tr("From Package parser : %1").arg(e);
            }
        }
        else
        {
            throw QObject::tr("Can not find Package tag !");
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
                    throw QObject::tr("From Update parser : %1").arg(e);
                }
                int release = upd.get_release();
                if(release > last_release){
                    last_update = upd;
                    last_release = release;
                }
                updates[release] = upd;
            }
        }
        else
        {
            throw QObject::tr("Can not find History tag !");
        }

        empty = false;
    }
    else
    {
        throw QObject::tr("Can not find PISI tag !");
    }
}

bool Pisi::save_to_dom(QDomDocument &dom) throw(QString)
{
    if(is_empty())
        throw QObject::tr("Empty pisi class while saving pisi class values to dom !");

    QDomElement root = dom.documentElement();

    if(root.isNull()){
        dom.clear();
        root = dom.createElement("PISI");
        dom.appendChild(root);
    }
    else{
        if(root.tagName().toLower() != "pisi")
            throw QObject::tr("Loaded xml file does not start with PISI !");
    }

    QDomElement elm_src = root.namedItem("Source").toElement();
    if(elm_src.isNull() || ! elm_src.isElement())
    {
        elm_src = root.ownerDocument().createElement("Source");
        root.appendChild(elm_src);
    }
    try {
        source.save_to_dom(elm_src);
    } catch (QString e) {
        throw QObject::tr("From Source saver : %1").arg(e);
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
        throw QObject::tr("From Package saver : %1").arg(e);
    }

    QDomElement elm_hist = root.namedItem("History").toElement();
    if( ! elm_hist.isNull() && elm_hist.isElement())
    {
        root.removeChild(elm_hist);
    }
    elm_hist = root.ownerDocument().createElement("History");
    root.appendChild(elm_hist);

    QList<int> releases = updates.keys();
    for(int i=releases.count()-1; i>=0; --i)
    {
        QDomElement elm_upd = root.ownerDocument().createElement("Update");
        elm_hist.appendChild(elm_upd);
        try {
            updates[releases.at(i)].save_to_dom(elm_upd);
        } catch (QString e) {
            throw QObject::tr("From Update saver : %1").arg(e);
        }
    }

    return true;
}
