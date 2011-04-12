#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include <QSettings>

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);
    read_settings();
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::read_settings()
{
    QSettings settings;
    settings.beginGroup( "configuration" );
    ui->le_pisi_archive_dir->setText(settings.value("pisi_archive_dir").toString());
    settings.endGroup();
}

void ConfigurationDialog::write_settings()
{
    QSettings settings;
    settings.beginGroup( "configuration" );
    settings.setValue("pisi_archive_dir", ui->le_pisi_archive_dir->text());
    settings.endGroup();
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    write_settings();
}
