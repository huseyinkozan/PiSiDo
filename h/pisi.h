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
    bool is_empty() const;

    void load_from_dom(const QDomDocument & dom);
    bool save_to_dom(QDomDocument & dom);

    PisiSource get_source() const;
    PisiPackage get_package() const;
    QMap<int, PisiUpdate> get_updates() const;
    PisiUpdate get_last_update() const;

    void set_source(PisiSource source);
    void set_package(PisiPackage package);
    void set_updates(QMap<int, PisiUpdate> updates);

private:
    bool empty;

    PisiSource source;
    PisiPackage package;
    QMap<int, PisiUpdate> updates;
    PisiUpdate last_update;
};

#endif // PISI_H
