#ifndef DIRECTORYMODEL_H
#define DIRECTORYMODEL_H

#include <QAbstractItemModel>

class DirectoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit DirectoryModel(QObject *parent = 0);

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

signals:

public slots:

};

#endif // DIRECTORYMODEL_H
