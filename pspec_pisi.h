#ifndef PSPECPISI_H
#define PSPECPISI_H

#include "pspec_source.h"
#include "pspec_package.h"
#include "pspec_update.h"
#include <QList>

class QDomDocument;

class PSpecPISI
{
public:
    PSpecPISI();

    void clear();
    bool is_loaded() { return loaded; }

    void load_from_dom(const QDomDocument & dom);
    bool save_to_dom(QDomDocument & dom);

    PSpecSource get_source();
    PSpecPackage get_package();
    QList<PSpecUpdate> get_updates();

private:
    bool loaded;

    PSpecSource source;
    PSpecPackage package;
    QList<PSpecUpdate> updates;
};

#endif // PSPECPISI_H
