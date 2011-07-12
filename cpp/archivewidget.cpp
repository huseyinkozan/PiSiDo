#include "archivewidget.h"
#include "ui_archivewidget.h"

ArchiveWidget::ArchiveWidget(QWidget *parent, QString archive, QString sha1) :
    QWidget(parent),
    ui(new Ui::ArchiveWidget)
{
    ui->setupUi(this);

    ui->le_archive->setText(archive);
    ui->le_sha1->setText(sha1);
}

ArchiveWidget::~ArchiveWidget()
{
    delete ui;
}

void ArchiveWidget::on_tb_delete_clicked()
{
    emit delete_me(this);
}

QString ArchiveWidget::get_archive()
{
    return ui->le_archive->text();
}

QString ArchiveWidget::get_sha1()
{
    return ui->le_sha1->text();
}
