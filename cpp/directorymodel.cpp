#include "directorymodel.h"

DirectoryModel::DirectoryModel(QDir rootDir, QObject *parent) :
    QAbstractItemModel(parent),
    rootDir(rootDir)
{
}

Qt::ItemFlags DirectoryModel::flags(const QModelIndex &/*index*/) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant DirectoryModel::data(const QModelIndex &index, int role) const
{
    if(role == Qt::DecorationRole){
//        if(folder)
        return QIcon(":/folder.png");
//        else
//        return QIcon(":/images/file.png");
    }
    else if(role == Qt::DisplayRole){
        return QString("DirOrFileName");
    }
    else if(role == Qt::ToolTipRole){
        return QString("Tooltip");
    }
    else if(role == Qt::StatusTipRole){
        return QString("StatusTip");
    }
    return QVariant();
}


QVariant DirectoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole){
        // section -> columns
        if(section == 0){
            return tr("File Or Directory");
        }
    }
    return QVariant();
}

int DirectoryModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid()){
        return 0; // return file and folder count
    }
    return 0;// invalid parent
}

int DirectoryModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 1;
}

QModelIndex DirectoryModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex DirectoryModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

void DirectoryModel::refresh()
{
}

