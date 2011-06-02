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
        throw QString("Dom Element is null while loading to PisiPackage !");

    QDomElement elm = dom_element.firstChildElement("RuntimeDependencies");
    runtime_dependencies = get_dep_from_element(elm, false);
}

void PisiPackage::save_to_dom(QDomElement & dom_element)
{
    // TODO : implement
    PisiSPBase::save_to_dom(dom_element);

    if(dom_element.isNull())
        throw QString("Dom Element is null while saving from PisiPackage to dom !");
}

QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr,QString> > PisiPackage::get_runtime_dependencies() const
{
    return runtime_dependencies;
}

QMultiMap<PisiPackage::FileType, QString> PisiPackage::get_files() const
{
    return files;
}

void PisiPackage::set_runtime_dependencies(QMap<QString, QMap<PisiSPBase::VersionReleaseToFromAttr, QString> > runtime_dependencies)
{
    // optional, no check
    this->runtime_dependencies = runtime_dependencies;
}

void PisiPackage::set_runtime_dependencies(QString runtime_dependency_string)
{
    QMap<QString, QMap<VersionReleaseToFromAttr, QString> > run_deps = get_dependency_list(runtime_dependency_string);
    set_runtime_dependencies(run_deps);
}

void PisiPackage::set_files(QMultiMap<PisiPackage::FileType, QString> files)
{
    this->files = files;
}

bool PisiPackage::operator ==(const PisiPackage & other)
{
    return (
                PisiSPBase::operator ==(other)
                && get_runtime_dependencies() == other.get_runtime_dependencies()
                && get_files() == other.get_files()
                );
}

bool PisiPackage::operator !=(const PisiPackage & other)
{
    return ! (*this == other);
}
