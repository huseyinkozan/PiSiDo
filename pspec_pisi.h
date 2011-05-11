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

    PSpecSource source;
    PSpecPackage package;
    QList<PSpecUpdate> updates;

    void load_from_dom(const QDomDocument & dom);
    bool save_to_dom(QDomDocument & dom);
};

#endif // PSPECPISI_H
