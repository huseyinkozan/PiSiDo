#include "pisipackage.h"

#include <QDomElement>

PisiPackage::PisiPackage()
    : PisiSPBase()
{
    clear();
}


void PisiPackage::clear()
{
    PisiSPBase::clear();
    runtime_dependencies.clear();
}

void PisiPackage::load_from_dom(const QDomElement & dom_element)
{
    PisiSPBase::load_from_dom(dom_element);

    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PspecPackage !");

    QDomElement elm = dom_element.firstChildElement("RuntimeDependencies");
    runtime_dependencies = get_dependency_map(elm, false);
}

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiPackage::get_runtime_dependencies()
{
    return runtime_dependencies;
}
