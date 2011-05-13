#ifndef PISI_H
#define PISI_H

#include "pisisource.h"
#include "pisipackage.h"
#include "pisiupdate.h"
#include <QMap>

class QDomDocument;

class Pisi
{
public:
    Pisi();

    void clear();
    bool is_empty();

    void load_from_dom(const QDomDocument & dom);
    bool save_to_dom(QDomDocument & dom);

    PisiSource get_source();
    PisiPackage get_package();
    QMap<int, PisiUpdate> get_updates();
    PisiUpdate get_last_update();

private:
    bool empty;

    PisiSource source;
    PisiPackage package;
    QMap<int, PisiUpdate> updates;
    PisiUpdate last_update;
};

#endif // PISI_H
