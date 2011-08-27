#ifndef ARCHIVEWIDGET_H
#define ARCHIVEWIDGET_H

#include <QWidget>

namespace Ui {
    class ArchiveWidget;
}

class ArchiveWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArchiveWidget(QWidget * parent,
                           const QString & archive,
                           const QString & sha1,
                           const QStringList & types,
                           const QString & selected_type);
    ~ArchiveWidget();

    QString get_archive();
    QString get_sha1();
    QString get_type();

signals:
    void delete_me(ArchiveWidget *);

private slots:
    void on_tb_delete_clicked();

private:
    Ui::ArchiveWidget *ui;
};

#endif // ARCHIVEWIDGET_H
