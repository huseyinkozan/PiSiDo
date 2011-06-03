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

    // TODO : do it for files !
}

void PisiPackage::save_to_dom(QDomElement & root_elm)
{
    PisiSPBase::save_to_dom(root_elm);

    if(root_elm.isNull())
        throw QString("Dom Element is null while saving from PisiPackage to dom !");

    QDomElement elm = root_elm.firstChildElement("RuntimeDependencies");
    if( ! elm.isNull())
        root_elm.removeChild(elm);
    elm = get_appended_dom_elm(root_elm, "RuntimeDependencies");
    set_dep_to_element(runtime_dependencies, elm, is_mandatory(root_elm, "RuntimeDependencies"));

    // TODO : do it for files !
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

bool PisiPackage::is_mandatory(QDomElement root, QString tag)
{
    if(tag == "RuntimeDependencies")
    {
        return false;
    }
    else if(tag == "Files")
    {
        return true;
    }
    else
        PisiSPBase::is_mandatory(root, tag);
}
