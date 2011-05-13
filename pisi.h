#ifndef PISI_H
#define PISI_H

#include "pisisource.h"
#include "pisipackage.h"
#include "pisiupdate.h"
#include <QList>

class QDomDocument;

class Pisi
{
public:
    Pisi();

    void clear();
    bool is_loaded() { return loaded; }

    void load_from_dom(const QDomDocument & dom);
    bool save_to_dom(QDomDocument & dom);

    PisiSource get_source();
    PisiPackage get_package();
    QList<PisiUpdate> get_updates();

private:
    bool loaded;

    PisiSource source;
    PisiPackage package;
    QList<PisiUpdate> updates;
};

#endif // PISI_H
