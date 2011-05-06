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

    ui->le_action_api_page->setText(settings.value("action_api_page").toString());
    ui->le_pisi_spec->setText(settings.value("pisi_spec").toString());
    ui->le_pisi_archive_dir->setText(settings.value("pisi_archive_dir").toString());
    ui->sb_folder_comp_time_limit->setValue(settings.value("folder_comp_time_limit").toInt());
    ui->chk_not_clear_packager->setChecked(settings.value("not_clear_packager").toBool());

    settings.endGroup();
}

void ConfigurationDialog::write_settings()
{
    QSettings settings;
    settings.beginGroup( "configuration" );

    settings.setValue("action_api_page", ui->le_action_api_page->text());
    settings.setValue("pisi_spec", ui->le_pisi_spec->text());
    settings.setValue("pisi_archive_dir", ui->le_pisi_archive_dir->text());
    settings.setValue("folder_comp_time_limit", ui->sb_folder_comp_time_limit->value());
    settings.setValue("not_clear_packager", ui->chk_not_clear_packager->isChecked());

    settings.endGroup();
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    write_settings();
}
