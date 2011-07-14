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
    files.clear();
}

void PisiPackage::load_from_dom(const QDomElement & dom_element)
{
    PisiSPBase::load_from_dom(dom_element);

    if(dom_element.isNull())
        throw QString("Dom Element is null while loading to PisiPackage !");

    QDomElement elm = dom_element.firstChildElement("RuntimeDependencies");
    runtime_dependencies = get_dependency(elm);

    elm = dom_element.firstChildElement("Files");
    files = get_files(elm);
}

void PisiPackage::save_to_dom(QDomElement & root)
{
    PisiSPBase::save_to_dom(root);

    if(root.isNull())
        throw QString("Dom Element is null while saving from PisiPackage to dom !");

    QDomElement elm = root.firstChildElement("RuntimeDependencies");
    if( ! elm.isNull())
        root.removeChild(elm);
    elm = append_element(root, "RuntimeDependencies");
    set_dependency(elm, runtime_dependencies);

    if( ! files.isEmpty())
        throw QString("%1 tag is mandatory but empty !").arg("Files");
    elm = root.firstChildElement("Files");
    if( ! elm.isNull())
        root.removeChild(elm);
    elm = append_element(root, "Files");
    set_files(elm, files);
}

QStringList PisiPackage::get_runtime_dependencies_as_stringlist()
{
    return get_dependency_list(runtime_dependencies);
}

QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > PisiPackage::get_runtime_dependencies() const
{
    return runtime_dependencies;
}

QMap<QString, QMap<PisiPackage::FileType, bool> > PisiPackage::get_files() const
{
    return files;
}

QMap<QString, QMap<QString, bool> > PisiPackage::get_files_as_string_type()
{
    QMap<QString, QMap<QString, bool> > result;
    QList<QString> files_keys = files.keys();
    foreach (QString file, files_keys) {
        QMap<FileType, bool> files_attr = files.value(file);
        QMap<QString, bool> result_attr;
        result_attr[get_files_file_type(files_attr.keys().first())] = files_attr.value(files_attr.keys().first());
        result[file] = result_attr;
    }
    return result;
}

void PisiPackage::set_runtime_dependencies(QMap<QString, QMap<PisiSPBase::VRTFAttr, QString> > runtime_dependencies)
{
    // optional, no check
    this->runtime_dependencies = runtime_dependencies;
}

void PisiPackage::set_runtime_dependencies(QString runtime_dependency_string)
{
    QMap<QString, QMap<VRTFAttr, QString> > run_deps = get_dependency_list(runtime_dependency_string);
    set_runtime_dependencies(run_deps);
}

void PisiPackage::set_files(QMap<QString, QMap<FileType, bool> > files)
{
    if(files.isEmpty()){
        QMap<FileType, bool> attr;
        attr[ALL] = false;
        this->files[QString("/")] = attr;
    }
    else{
        this->files = files;
    }

}

void PisiPackage::set_files(QMap<QString, QMap<QString, bool> > files)
{
    this->files.clear();
    if(files.isEmpty()){
        QMap<FileType, bool> attr;
        attr[ALL] = false;
        this->files[QString("/")] = attr;
    }
    else{
        QList<QString> keys = files.keys();
        foreach (QString path, keys) {
            QMap<QString, bool> value = files.value(path);
            QMap<FileType, bool> attr;
            attr[get_files_file_type(value.keys().first())] = value.value(value.keys().first());
            this->files[path] = attr;
        }
    }
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
        return PisiSPBase::is_mandatory(root, tag);
}

QMap<QString, QMap<PisiPackage::FileType, bool> > PisiPackage::get_files(QDomElement elm)
{
    if(elm.isNull())
    {
        return QMap<QString, QMap<FileType, bool> >();
    }

    QMap<QString, QMap<FileType, bool> > files;

    elm = elm.firstChildElement("Path");
    if(elm.isNull())
        throw QString("No Path in Files");

    for( ; ! elm.isNull(); elm = elm.nextSiblingElement("Path"))
    {
        QMap<FileType, bool> attributes;
        QString file_type = elm.attribute("fileType");
        if(file_type.isNull())
            throw QString("There is no fileType attribute in Path tag !");
        bool permanent = false;
        if(elm.attribute("permanent").toLower() == "true")
            permanent = true;
        attributes[get_files_file_type(file_type)] = permanent;
        files[elm.text()] = attributes;
    }
    return files;
}

void PisiPackage::set_files(QDomElement root, QMap<QString, QMap<FileType, bool> > files)
{
    if(files.isEmpty())
    {
        throw QString("No %1 tag !").arg("Files");
    }

    QList<QString> files_keys = files.keys();
    for(int i=0; i<files_keys.count(); ++i)
    {
        QString path = files_keys.at(i);
        QDomElement elm = append_element(root, "Path");
        append_text_element(elm, path);

        QMap<FileType, bool> attr = files[path];
        FileType file_type = attr.keys().first();
        elm.setAttribute("fileType", get_files_file_type(file_type));
        elm.setAttribute("permanent", attr[file_type]);
    }
}

QString PisiPackage::get_files_file_type(PisiPackage::FileType attr)
{
    switch(attr)
    {
    case EXECUTABLE:
        return "executable";
        break;
    case LIBRARY:
        return "library";
        break;
    case DATA:
        return "data";
        break;
    case CONFIG:
        return "config";
        break;
    case DOC:
        return "doc";
        break;
    case MAN:
        return "man";
        break;
    case INFO:
        return "info";
        break;
    case LOCALEDATA:
        return "localedata";
        break;
    case HEADER:
        return "header";
        break;
    case ALL:
        return "all";
        break;
    default:
        return "";
        break;
    }
}

PisiPackage::FileType PisiPackage::get_files_file_type(QString attr_name)
{
        if(attr_name.toLower() == "executable")
            return EXECUTABLE;
        else if(attr_name.toLower() == "library")
            return LIBRARY;
        else if(attr_name.toLower() == "data")
            return DATA;
        else if(attr_name.toLower() == "config")
            return CONFIG;
        else if(attr_name.toLower() == "doc")
            return DOC;
        else if(attr_name.toLower() == "man")
            return MAN;
        else if(attr_name.toLower() == "info")
            return INFO;
        else if(attr_name.toLower() == "localedata")
            return LOCALEDATA;
        else if(attr_name.toLower() == "header")
            return HEADER;
        else if(attr_name.toLower() == "all")
            return ALL;
        else
            throw QString("Wrong files atribute name : %1").arg(attr_name);
}
