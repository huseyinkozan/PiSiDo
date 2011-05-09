#ifndef PSPECPISI_H
#define PSPECPISI_H

#include "pspecsource.h"
#include "pspecpackage.h"
#include "pspecupdate.h"
#include <QList>

class PSpecPISI
{
public:
    PSpecPISI();

    PSpecSource source;
    PSpecPackage package;
    QList<PSpecUpdate> updates;
};

#endif // PSPECPISI_H
