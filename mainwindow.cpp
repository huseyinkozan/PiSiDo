#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QSettings>
#include <QUrl>
#include <QXmlStreamWriter>
#include <QCloseEvent>
#include <QCryptographicHash>
#include <QProcess>
#include <QDate>

#include <configurationdialog.h>

#include <QDebug>

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

    read_settings();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
     write_settings();
     event->accept();
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
    QString src_compressed = get_user_selection(File, "source", "src_compressed", this, tr("Select Compressed File"), filter);
    ui->le_src_compressed->setText(src_compressed);
}

void MainWindow::on_tb_src_folder_clicked()
{
    QString src_folder = get_user_selection(Folder, "source", "src_folder", this, tr("Select Directory"));
    ui->le_src_folder->setText(src_folder);
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

void MainWindow::write_settings()
{
    QSettings settings;
    settings.beginGroup("source");
    foreach(QObject * item, ui->gb_src->children())
    {
        if(QRadioButton * btn = qobject_cast<QRadioButton *>(item))
        {
            if(btn->isChecked())
                settings.setValue("selected_source", btn->objectName());
        }
    }
    settings.setValue("src_compressed", ui->le_src_compressed->text());
    settings.setValue("src_folder", ui->le_src_folder->text());
    settings.setValue("src_url", ui->le_src_url->text());
    settings.setValue("src_home_page", ui->le_src_home_page->text());
    settings.setValue("src_sub_dir", ui->le_src_sub_dir->text());
    settings.setValue("cp_to_pckg_work", ui->chk_cp_to_pckg_work->isChecked());
    settings.setValue("cp_to_pisi_work", ui->chk_cp_to_pisi_work->isChecked());
    settings.endGroup();
    settings.beginGroup("package");
    settings.setValue("name", ui->le_name->text());
    settings.setValue("version", ui->le_version->text());
    settings.setValue("other_licence", ui->le_other_licence->text());
    settings.setValue("work_dir", ui->le_work_dir->text());
    settings.setValue("brief", ui->le_brief->text());
    settings.setValue("detailed", ui->te_detailed->toPlainText());
    settings.setValue("licence", ui->combo_licence->currentText());
    settings.setValue("type", ui->combo_type->currentText());
    settings.setValue("component_group", ui->combo_component_group->currentText());
    settings.endGroup();
    settings.beginGroup("compilation");
    foreach(QObject * item, ui->gb_action_modules->children())
    {
        if(QRadioButton * btn = qobject_cast<QRadioButton *>(item))
        {
            if(btn->isChecked())
                settings.setValue("action_module", btn->objectName());
        }
    }
    settings.setValue("create_comar", ui->chk_create_comar->isChecked());
    settings.setValue("create_desktop", ui->chk_create_desktop->isChecked());
    settings.setValue("create_icon", ui->chk_create_icon->isChecked());
    settings.setValue("create_service", ui->chk_create_service->isChecked());
    settings.endGroup();
    settings.beginGroup("packager");
    settings.setValue("packager_name", ui->le_packager_name->text());
    settings.setValue("packager_email", ui->le_packager_email->text());
    settings.endGroup();
}

void MainWindow::read_settings()
{
    QSettings settings;
    settings.beginGroup("source");
    if( ! settings.value("selected_source").toString().isEmpty())
    {
        foreach(QObject * obj, ui->gb_src->children())
        {
            if(QRadioButton * btn = qobject_cast<QRadioButton *>(obj))
            {
                if(btn->objectName() == settings.value("selected_source").toString())
                    btn->setChecked(true);
            }
        }
    }
    ui->le_src_compressed->setText(settings.value("src_compressed").toString());
    ui->le_src_folder->setText(settings.value("src_folder").toString());
    ui->le_src_url->setText(settings.value("src_url").toString());
    ui->le_src_home_page->setText(settings.value("src_home_page").toString());
    ui->le_src_sub_dir->setText(settings.value("src_sub_dir").toString());
    ui->chk_cp_to_pckg_work->setChecked(settings.value("cp_to_pckg_work", false).toBool());
    ui->chk_cp_to_pisi_work->setChecked(settings.value("cp_to_pisi_work", false).toBool());
    settings.endGroup();
    settings.beginGroup("package");
    ui->le_name->setText(settings.value("name").toString());
    ui->le_version->setText(settings.value("version").toString());
    ui->le_other_licence->setText(settings.value("other_licence").toString());
    ui->le_work_dir->setText(settings.value("work_dir").toString());
    ui->le_brief->setText(settings.value("brief").toString());
    ui->te_detailed->setPlainText(settings.value("detailed").toString());
    ui->combo_licence->setCurrentIndex(ui->combo_licence->findText(settings.value("licence","").toString()));
    ui->combo_type->setCurrentIndex(ui->combo_type->findText(settings.value("type","").toString()));
    ui->combo_component_group->setCurrentIndex(ui->combo_component_group->findText(settings.value("component_group","").toString()));
    settings.endGroup();
    settings.beginGroup("compilation");
    if( ! settings.value("action_module").toString().isEmpty())
    {
        foreach(QObject * obj, ui->gb_action_modules->children())
        {
            if(QRadioButton * btn = qobject_cast<QRadioButton *>(obj))
            {
                if(btn->objectName() == settings.value("action_module").toString())
                    btn->setChecked(true);
            }
        }
    }
    ui->chk_create_comar->setChecked(settings.value("create_comar", false).toBool());
    ui->chk_create_desktop->setChecked(settings.value("create_desktop", false).toBool());
    ui->chk_create_icon->setChecked(settings.value("create_icon", false).toBool());
    ui->chk_create_service->setChecked(settings.value("create_service", false).toBool());
    settings.endGroup();
    settings.beginGroup("packager");
    ui->le_packager_name->setText(settings.value("packager_name").toString());
    ui->le_packager_email->setText(settings.value("packager_email").toString());
    settings.endGroup();
}



void MainWindow::on_pb_create_clicked()
{
    QString str_work_dir = ui->le_work_dir->text();
    QFileInfo work_dir_info(str_work_dir);
    if( str_work_dir.isEmpty() || ! work_dir_info.exists() || ! work_dir_info.isWritable())
    {
        QMessageBox::critical(this, tr("Error"), tr("No writable work dir. Please define a writable work dir."));
        return;
    }
    QString package_name = ui->le_name->text();
    if( package_name.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No package name. Please define package name."));
        return;
    }

    QDir work_dir(str_work_dir);
    if( ! work_dir.cd(package_name) )
    {
        if( ! work_dir.mkdir(package_name) )
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not create package dir."));
            return;
        }
        work_dir.cd(package_name);
    }
    QString file_name = work_dir.absoluteFilePath("pspec.xml");
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open file to write."));
        return;
    }

    QString src_path;

    if(ui->rb_src_compressed->isChecked())
    {
        src_path = ui->le_src_compressed->text();
    }
    else if(ui->rb_src_url->isChecked())
    {
        src_path = ui->le_src_url->text();
        if(ui->chk_cp_to_pckg_work->isChecked())
        {
            // TODO: process download with internal classes
            // download with wget
            QUrl url(QUrl::fromUserInput(src_path));
            QFileInfo file_info(url.path());
            if(QFile::exists(work_dir.absoluteFilePath(file_info.fileName())))
                QFile::remove(work_dir.absoluteFilePath(file_info.fileName()));
            QDir application_dir = QDir::current();
            QDir::setCurrent(work_dir.absolutePath());
            QProcess xterm;
            QStringList parameters;
            parameters << "-e" << "wget" << src_path;
            xterm.start("xterm", parameters);
            statusBar()->showMessage(tr("Downloading archive ......"));
            if (!xterm.waitForFinished(-1))
            {
                QMessageBox::information(this, tr("Error"), tr("Download archive failed."));
            }
            statusBar()->showMessage(tr("Archive download finished."));
            QDir::setCurrent(application_dir.absolutePath());
            src_path = work_dir.absoluteFilePath(file_info.fileName());
        }
    }
    else if (ui->rb_src_folder)
    {
        src_path = ui->le_src_folder->text();
        // compress and copy to work dir
        QDir src_path_dir(src_path);
        QString dir_name = src_path_dir.dirName();
        src_path = work_dir.absoluteFilePath(QString("%1.tar.gz").arg(dir_name));
        src_path_dir.cdUp();
        QProcess tar;
        QStringList parameters;
        parameters << "-zcf" << src_path;
        parameters << "--directory" << src_path_dir.absolutePath() << dir_name;
        tar.start("tar", parameters);
        statusBar()->showMessage(tr("Compressing directory ......"));
        if (!tar.waitForFinished(5*60*1000))        // 5 minutes
        {
            QMessageBox::critical(this, tr("Error"), tr("Source folder could not compress."));
            return;
        }
        statusBar()->showMessage(tr("Directory compression finished."));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Source not selected."));
        return;
    }

    QString src_file_type;
    if(src_path.endsWith(".tar.gz"))
        src_file_type = "targz";
    else if(src_path.endsWith(".tar.bz2"))
        src_file_type = "tarbz2";
    else if(src_path.endsWith(".tar.lzma"))
        src_file_type = "tarlzma";
    else if(src_path.endsWith(".tar.xz"))
        src_file_type = "tarxz";
    else if(src_path.endsWith(".tar.Z"))
        src_file_type = "tarZ";
    else if(src_path.endsWith(".tar"))
        src_file_type = "tar";
    else if(src_path.endsWith(".zip"))
        src_file_type = "zip";
    else if(src_path.endsWith(".gz"))
        src_file_type = "gz";
    else if(src_path.endsWith(".gzip"))
        src_file_type = "gzip";
    else if(src_path.endsWith(".bz2"))
        src_file_type = "bz2";
    else if(src_path.endsWith(".bzip2"))
        src_file_type = "bzip2";
    else if(src_path.endsWith(".lzma"))
        src_file_type = "lzma";
    else if(src_path.endsWith(".xz"))
        src_file_type = "xz";
    else
        src_file_type = "binary";

    QFile compressed_file(src_path);

    if( ! compressed_file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open file to sha1sum."));
        return;
    }
    QByteArray compressed_file_bytes = compressed_file.readAll();
    QString sha1sum = QCryptographicHash::hash(compressed_file_bytes, QCryptographicHash::Sha1).toHex();
    compressed_file_bytes.clear();
    compressed_file.close();

    if(ui->chk_cp_to_pisi_work->isChecked())
    {
        QSettings settings;
        settings.beginGroup( "configuration" );
        QString pisi_dir = settings.value("pisi_archive_dir").toString();
        settings.endGroup();
        QString cmd = QString("/usr/bin/xdg-su -u root -c /bin/cp \"%1\" %2").arg(src_path).arg(pisi_dir);
        system(qPrintable(cmd));
    }

    QString src_home_page = ui->le_src_home_page->text();
    QString component_group = ui->combo_component_group->currentText();
    QString brief = ui->le_brief->text();
    QString detailed = ui->te_detailed->toPlainText();
    QString archive_name = QFileInfo(src_path).fileName();
    int release = 1;
    QString release_date = QDate::currentDate().toString("yyyy-MM-dd");
    QString release_version = ui->le_version->text();
    QString release_comment = tr("First Release");
    QString packager_name = ui->le_packager_name->text();
    QString packager_email = ui->le_packager_email->text();

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.writeStartDocument("1.0");
    xml.writeDTD("<!DOCTYPE PISI SYSTEM \"http://www.pardus.org.tr/projeler/pisi/pisi-spec.dtd\">");

    xml.writeStartElement("PISI");
        xml.writeStartElement("Source");
            xml.writeTextElement("Name", package_name);
            xml.writeTextElement("Homepage", src_home_page);
            xml.writeTextElement("PartOf", component_group);
            xml.writeTextElement("Summary", brief);
            xml.writeComment(QString::fromUtf8("<Summary xml:lang=\"tr\">Özet (Türkçe) </Summary>"));
            xml.writeTextElement("Description", detailed);
            xml.writeComment(QString::fromUtf8("<Description xml:lang=\"tr\"> Paketin tanıtımı (Türkçe)</Description>"));
            xml.writeStartElement("Archive");
                xml.writeAttribute("type",src_file_type);
                xml.writeAttribute("sha1sum",sha1sum);
                xml.writeCharacters(archive_name);
            xml.writeEndElement(); // Archive
            xml.writeComment(QString::fromUtf8("<BuildDependencies>"));
            xml.writeComment(QString::fromUtf8("  <Dependency>Varsa derleme bağımlılığı paketi</Dependency>"));
            xml.writeComment(QString::fromUtf8("</BuildDependencies>"));
            xml.writeComment(QString::fromUtf8("<Patches>"));
            xml.writeComment(QString::fromUtf8("  <Patch>Varsa derleme bağımlılığı yaması</Patch>"));
            xml.writeComment(QString::fromUtf8("</Patches>"));
        xml.writeEndElement(); // Source
        xml.writeStartElement("Package");
            xml.writeTextElement("Name", package_name);
            xml.writeComment(QString::fromUtf8("<RuntimeDependencies>"));
            xml.writeComment(QString::fromUtf8("  <Dependency versionFrom=\"sürüm numarası\">Varsa çalıştırma bağımlılığı paketi</Dependency>"));
            xml.writeComment(QString::fromUtf8("</RuntimeDependencies>"));
            xml.writeStartElement("Files");
                xml.writeStartElement("Path");
                    xml.writeAttribute("fileType", "all");
                    xml.writeCharacters("/");
                xml.writeEndElement(); // Path
                xml.writeComment(QString::fromUtf8("<Path fileType=\"executable\">/usr/bin</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"config\">/etc</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"library\">/usr/lib</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"header\">/usr/include</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"data\">/usr/share</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"localedata\">/usr/share/locale</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"man\">/usr/share/man</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"info\">/usr/share/info</Path>"));
                xml.writeComment(QString::fromUtf8("<Path fileType=\"doc\">/usr/share/doc</Path>"));
            xml.writeEndElement(); // Files
            // TODO : process file list
            xml.writeComment(QString::fromUtf8("<AdditionalFiles>"));
            xml.writeComment(QString::fromUtf8("  <AdditionalFile owner=\"root\" permission=\"0644\" target=\"/usr/share/applications/heimdall.desktop\">heimdall.desktop</AdditionalFile>"));
            xml.writeComment(QString::fromUtf8("</AdditionalFiles>"));
            xml.writeComment(QString::fromUtf8("<Provides>"));
            xml.writeComment(QString::fromUtf8("  <COMAR script=\"package.py\">System.Package</COMAR>"));
            xml.writeComment(QString::fromUtf8("  <COMAR script=\"service.py\">System.Service</COMAR>"));
            xml.writeComment(QString::fromUtf8("</Provides>"));
        xml.writeEndElement(); // Package
        xml.writeStartElement("History");
            // TODO : read history and produce new entries
            xml.writeStartElement("Update");
                xml.writeAttribute("release", QString::number(release));
                xml.writeTextElement("Date", release_date);
                xml.writeTextElement("Version", release_version);
                xml.writeTextElement("Comment", release_comment);
                xml.writeTextElement("Name", packager_name);
                xml.writeTextElement("Email", packager_email);
            xml.writeEndElement(); // Update
        xml.writeEndElement(); // History
    xml.writeEndElement(); // PISI

    xml.writeEndDocument();

    QMessageBox::information(this, "","created");
}



