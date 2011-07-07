#include "addupdatedialog.h"
#include "ui_addupdatedialog.h"

#include <QSettings>

AddUpdateDialog::AddUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUpdateDialog)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("packager");
    ui->le_packager_name->setText(settings.value("packager_name").toString());
    ui->le_packager_email->setText(settings.value("packager_email").toString());
    settings.endGroup();

    ui->de_date->setDate(QDate::currentDate());
}

AddUpdateDialog::~AddUpdateDialog()
{
    delete ui;
}

QString AddUpdateDialog::get_date()
{
    return ui->de_date->date().toString("dd.MM.yyyy");
}

QString AddUpdateDialog::get_version()
{
    return ui->le_update_version->text();
}

QString AddUpdateDialog::get_comment()
{
    return ui->le_update_comment->text();
}

QString AddUpdateDialog::get_packager_name()
{
    return ui->le_packager_name->text();
}

QString AddUpdateDialog::get_packager_email()
{
    return ui->le_packager_email->text();
}
