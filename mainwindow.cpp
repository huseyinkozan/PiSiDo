#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>

#include <configurationdialog.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // read and write settings if there is no setting entry
    QSettings settings;
    settings.beginGroup( "configuration" );
    QString pisi_archive_dir = settings.value("pisi_archive_dir", QString("/var/cache/pisi/archives/")).toString();
    settings.setValue("pisi_archive_dir", pisi_archive_dir);
    settings.endGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About"), trUtf8("This program written by Hüseyin Kozan."
                                            "\nhttp://huseyinkozan.com.tr"
                                            "\nApp Version:%1").arg(qApp->applicationVersion()));
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

/**
  gets user file or folder selection. Saves selected directory for next call.
  */

QString MainWindow::get_user_selection( User_Selection_Mode selection_mode, QString setting_group, QString setting_key,
                                       QWidget * parent, QString title, QString file_filter )
{
    QSettings settings;
    if( ! setting_group.isEmpty())
        settings.beginGroup( setting_group );
    QString selection_dir = settings.value( QString("%1_dir").arg(setting_key),
                                           QDesktopServices::storageLocation(QDesktopServices::HomeLocation)).toString();
    QString selected;
    if(selection_mode == Folder)
        selected = QFileDialog::getExistingDirectory(parent, title,selection_dir);
    else
        selected = QFileDialog::getOpenFileName(parent, title, selection_dir, file_filter);

    if( ! selected.isEmpty())
    {
        if( ! setting_key.isEmpty())
        {
            settings.setValue(QString("%1_dir").arg(setting_key), QFileInfo(selected).dir().absolutePath());
            settings.setValue(setting_key, selected);
        }
        if( ! setting_group.isEmpty())
            settings.endGroup();
        return selected;
    }
    else
    {
        if( ! setting_group.isEmpty())
            settings.endGroup();
        return QString();
    }
}

void MainWindow::on_tb_src_compressed_clicked()
{
    QString filter = tr("Compressed Files (*.targz *.tarbz2 *.tarlzma *.tarxz *.tarZ *.tar *.zip *.gz *.gzip *.bz2 *.bzip2 *.lzma *.xz *.binary)");
    QString source_compressed_file = get_user_selection(File, "source", "source_compressed_file", this, tr("Select Compressed File"), filter);
    ui->le_src_compressed->setText(source_compressed_file);
}

void MainWindow::on_tb_src_folder_clicked()
{
    QString source_folder = get_user_selection(Folder, "source", "source_folder", this, tr("Select Directory"));
    ui->le_src_folder->setText(source_folder);
}

void MainWindow::on_combo_licence_currentIndexChanged(int index)
{
    ui->le_other_licence->setEnabled(index == 0);
}

void MainWindow::on_tb_work_dir_clicked()
{
    QString work_dir = get_user_selection(Folder, "package", "work_dir", this, tr("Select Directory"), "");
    ui->le_work_dir->setText(work_dir);
}

void MainWindow::on_pb_add_file_clicked()
{
    QString file_path = get_user_selection(File, "package", "last_added", this, tr("Add File"));
    QFileInfo fi(file_path);
    if(package_files.contains(fi.fileName()))
        return;
    package_files.insert(fi.fileName(), file_path);
    ui->lw_files->addItem(fi.fileName());
}

void MainWindow::on_pb_remove_file_clicked()
{
    foreach(QListWidgetItem * item, ui->lw_files->selectedItems())
    {
        package_files.remove(item->text());
        delete ui->lw_files->takeItem(ui->lw_files->row(item));
    }
}

void MainWindow::on_pb_clear_files_clicked()
{
    package_files.clear();
}

void MainWindow::on_actionConfigure_Application_triggered()
{
    ConfigurationDialog cd(this);
    if(cd.exec() == QDialog::Accepted)
    {
        // apply settings if needed.
    }
}

void MainWindow::on_actionOpen_Help_Page_triggered()
{
    QDesktopServices::openUrl(QUrl("http://huseyinkozan.com.tr"));
}




void MainWindow::on_pb_create_clicked()
{
    // -/-
}
