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
    ui->le_packager_name->setText(settings.value("packager_email").toString());
    settings.endGroup();
}

AddUpdateDialog::~AddUpdateDialog()
{
    delete ui;
}
