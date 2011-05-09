#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QCloseEvent>
#include <QCryptographicHash>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>

#include "configurationdialog.h"
#include "helpdialog.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    help_dialog(NULL)
{
    ui->setupUi(this);

    // fill comboboxes
    ui->combo_is_a->addItem("");
    ui->combo_is_a->addItems(get_file_strings(":/files/is_a"));
    ui->combo_licence->addItem("");
    ui->combo_licence->addItems(get_file_strings(":/files/license"));
    ui->combo_part_of->addItem("");
    ui->combo_part_of->addItems(get_file_strings(":/files/part_of"));

    // set update date
    ui->de_update_date->setDate(QDate::currentDate());

    // read and write settings if there is no setting entry

    // action_api_page : http://tr.pardus-wiki.org/Pardus:ActionsAPI
    // pisi_spec : http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng
    // pisi_archive_dir : /var/cache/pisi/archives
    // folder_comp_time_limit : 2
    // not_clear_packager : true

    bool ok = false;

    settings.beginGroup( "configuration" );

    QString action_api_page = settings.value("action_api_page", QString("http://tr.pardus-wiki.org/Pardus:ActionsAPI")).toString();
    QString pisi_spec = settings.value("pisi_spec", QString("http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng")).toString();
    QString pisi_archive_dir = settings.value("pisi_archive_dir", QString("/var/cache/pisi/archives/")).toString();
    bool not_clear_packager = settings.value("not_clear_packager", true).toBool();
    int folder_comp_time_limit = settings.value("folder_comp_time_limit", 2).toInt(&ok);
    if(!ok) folder_comp_time_limit = 2;

    settings.setValue("action_api_page", action_api_page);
    settings.setValue("pisi_spec", pisi_spec);
    settings.setValue("pisi_archive_dir", pisi_archive_dir);
    settings.setValue("folder_comp_time_limit", folder_comp_time_limit);
    settings.setValue("not_clear_packager", not_clear_packager);

    settings.endGroup();

    action_defaults.clear();
    action_defaults.insert(0, ui->te_auto->toHtml());
    action_defaults.insert(1, ui->te_cmake->toHtml());
    action_defaults.insert(2, ui->te_kde4->toHtml());
    action_defaults.insert(3, ui->te_qt4->toHtml());
    action_defaults.insert(4, ui->te_python->toHtml());
    action_defaults.insert(5, ui->te_scons->toHtml());

    desktop_file_default = ui->pte_desktop->toPlainText();

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
    QMessageBox::about(this, tr("About"), trUtf8("This program developed by Hüseyin Kozan."
                                            "\n\nE-Mail : posta@huseyinkozan.com.tr"
                                            "\nWeb : http://huseyinkozan.com.tr"
                                            "\n\nApplication Version:%1").arg(qApp->applicationVersion()));
}

void MainWindow::on_action_About_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}


QStringList MainWindow::get_file_strings(const QString & file_name)
{
    QFile file(file_name);
    if( ! file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open \"%1\" resource !").arg(file_name));
        return QStringList();
    }
    QStringList list;
    QTextStream stream(&file);
    while( ! stream.atEnd())
    {
        QString line = stream.readLine().trimmed();
        if( ! line.isEmpty())
            list.append(line);
    }
    file.close();
    return list;
}



/**
  gets user file or folder selection. Saves selected directory for next call.
  */

QString MainWindow::get_user_selection( User_Selection_Mode selection_mode, QString setting_group, QString setting_key,
                                       QWidget * parent, QString title, QString file_filter )
{
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
    if( ! src_compressed.isEmpty())
        ui->le_src_compressed->setText(src_compressed);
}

void MainWindow::on_tb_src_folder_clicked()
{
    QString src_folder = get_user_selection(Folder, "source", "src_folder", this, tr("Select Directory"));
    if( ! src_folder.isEmpty())
        ui->le_src_folder->setText(src_folder);
}

void MainWindow::on_pb_work_dir_browse_clicked()
{
    QString work_dir = get_user_selection(Folder, "package", "work_dir", this, tr("Select Directory"), "");
    if( ! work_dir.isEmpty())
        ui->le_work_dir->setText(work_dir);
}

void MainWindow::on_pb_work_dir_open_clicked()
{
    QString work_dir_str = ui->le_work_dir->text();
    if( ! work_dir_str.isEmpty())
    {
        QDir work_dir(work_dir_str);
        if(work_dir.exists())
            QDesktopServices::openUrl(QUrl(work_dir_str));
    }
}

void MainWindow::on_actionConfigure_Application_triggered()
{
    ConfigurationDialog cd(this);
    if(cd.exec() == QDialog::Accepted)
    {
        // apply settings if needed.
    }
}

void MainWindow::on_action_Help_triggered()
{
    if(help_dialog == NULL)
    {
        help_dialog = new HelpDialog(this);
        help_dialog->show();
    }
    help_dialog->show();
    help_dialog->raise();
    help_dialog->activateWindow();
}

void MainWindow::write_settings()
{
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
    settings.setValue("src_sha1", ui->le_src_sha1->text());
    settings.setValue("src_home_page", ui->le_src_home_page->text());
    settings.setValue("cp_to_pisi_archive", ui->chk_cp_to_pisi_archive->isChecked());
    settings.endGroup();
    settings.beginGroup("package");
    settings.setValue("name", ui->le_package_name->text());
    settings.setValue("version", ui->le_version->text());
    settings.setValue("work_dir", ui->le_work_dir->text());
    settings.setValue("brief", ui->le_brief->text());
    settings.setValue("detailed", ui->te_detailed->toPlainText());
    settings.setValue("licence", ui->combo_licence->currentText());
    settings.setValue("is_a", ui->combo_is_a->currentText());
    settings.setValue("part_of", ui->combo_part_of->currentText());
    settings.setValue("build_dependency", ui->le_build_dependency->text());
    settings.setValue("runtime_dependency", ui->le_runtime_dependency->text());
    settings.endGroup();
    settings.beginGroup("compilation");
    settings.setValue("action_template", ui->lw_action_template->currentIndex().row());
    settings.setValue("create_desktop", ui->chk_create_desktop->isChecked());
    settings.setValue("desktop_file", ui->pte_desktop->toPlainText());
    settings.setValue("te_auto", ui->te_auto->toHtml());
    settings.setValue("te_cmake", ui->te_cmake->toHtml());
    settings.setValue("te_kde4", ui->te_kde4->toHtml());
    settings.setValue("te_qt4", ui->te_qt4->toHtml());
    settings.setValue("te_python", ui->te_python->toHtml());
    settings.setValue("te_scons", ui->te_scons->toHtml());
    settings.endGroup();
    settings.beginGroup("packager");
    settings.setValue("packager_name", ui->le_packager_name->text());
    settings.setValue("packager_email", ui->le_packager_email->text());
    settings.endGroup();
}

void MainWindow::read_settings()
{
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
    QString src_compressed = settings.value("src_compressed").toString();
    if( ! QFile::exists(src_compressed))
        src_compressed.clear();
    QString src_folder = settings.value("src_folder").toString();
    QDir dir_src_folder(src_folder);
    if( ! dir_src_folder.exists())
        src_folder.clear();
    ui->le_src_compressed->setText(src_compressed);
    ui->le_src_folder->setText(src_folder);
    ui->le_src_url->setText(settings.value("src_url").toString());
    ui->le_src_sha1->setText(settings.value("src_sha1").toString());
    ui->le_src_home_page->setText(settings.value("src_home_page").toString());
    ui->chk_cp_to_pisi_archive->setChecked(settings.value("cp_to_pisi_archive", false).toBool());
    settings.endGroup();
    settings.beginGroup("package");
    ui->le_work_dir->setText(settings.value("work_dir").toString());
    ui->le_package_name->setText(settings.value("name").toString());
    ui->le_version->setText(settings.value("version").toString());
    ui->le_brief->setText(settings.value("brief").toString());
    ui->te_detailed->setPlainText(settings.value("detailed").toString());
    ui->combo_licence->setCurrentIndex(ui->combo_licence->findText(settings.value("licence","").toString()));
    ui->combo_is_a->setCurrentIndex(ui->combo_is_a->findText(settings.value("is_a","").toString()));
    ui->combo_part_of->setCurrentIndex(ui->combo_part_of->findText(settings.value("part_of","").toString()));
    ui->le_build_dependency->setText(settings.value("build_dependency").toString());
    ui->le_runtime_dependency->setText(settings.value("runtime_dependency").toString());
    settings.endGroup();
    settings.beginGroup("compilation");
    ui->lw_action_template->setCurrentRow(settings.value("action_template",0).toInt());
    ui->sw_action_template->setCurrentIndex(settings.value("action_template",0).toInt());
    ui->chk_create_desktop->setChecked(settings.value("create_desktop", false).toBool());
    ui->pte_desktop->setPlainText(settings.value("desktop_file", desktop_file_default).toString());
    ui->te_auto->setHtml(settings.value("te_auto", action_defaults.at(0)).toString());
    ui->te_cmake->setHtml(settings.value("te_cmake", action_defaults.at(1)).toString());
    ui->te_kde4->setHtml(settings.value("te_kde4", action_defaults.at(2)).toString());
    ui->te_qt4->setHtml(settings.value("te_qt4", action_defaults.at(3)).toString());
    ui->te_python->setHtml(settings.value("te_python", action_defaults.at(4)).toString());
    ui->te_scons->setHtml(settings.value("te_scons", action_defaults.at(5)).toString());
    settings.endGroup();
    settings.beginGroup("packager");
    ui->le_packager_name->setText(settings.value("packager_name").toString());
    ui->le_packager_email->setText(settings.value("packager_email").toString());
    settings.endGroup();
}



void MainWindow::on_pb_clear_clicked()
{
    settings.beginGroup("configuration");
    bool not_clear_packager = settings.value("not_clear_packager").toBool();
    settings.endGroup();

    QList<QLineEdit *> le_list = findChildren<QLineEdit *>();
    foreach(QLineEdit * le, le_list)
        if(le == ui->le_packager_name || le == ui->le_packager_email)
        {
            if( ! not_clear_packager)
                le->clear();
        }
        else
            le->clear();

    QList<QComboBox *> cb_list = findChildren<QComboBox *>();
    foreach(QComboBox * cb, cb_list)
        cb->setCurrentIndex(0);

    QList<QCheckBox *> chkb_list = findChildren<QCheckBox *>();
    foreach(QCheckBox * chkb, chkb_list)
        chkb->setChecked(false);

    ui->te_detailed->clear();

    ui->te_auto->setText(action_defaults.at(0));
    ui->te_cmake->setText(action_defaults.at(1));
    ui->te_kde4->setText(action_defaults.at(2));
    ui->te_qt4->setText(action_defaults.at(3));
    ui->te_python->setText(action_defaults.at(4));
    ui->te_scons->setText(action_defaults.at(5));
}



QString MainWindow::get_archive_type(const QString & src_path)
{
    QString src_file_type;
    if(src_path.endsWith(".tgz") || src_path.endsWith(".tar.gz"))
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
    return src_file_type;
}


QDir MainWindow::get_package_dir(QDir work_dir, QString package_name)
{
    if( ! work_dir.cd(package_name) )
    {
        if( ! work_dir.mkdir(package_name) )
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not create package dir in the pisido work dir."));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
            ui->pb_work_dir_browse->setFocus();
            return QDir();
        }
        work_dir.cd(package_name);
    }
    return work_dir;
}

void MainWindow::on_pb_create_clicked()
{
    QString work_dir_str = ui->le_work_dir->text();
    QFileInfo work_dir_info(work_dir_str);

    if( work_dir_str.isEmpty() || ! work_dir_info.exists())
    {
        QMessageBox::critical(this, tr("Error"), tr("No pisido work directory. Please define a working directory."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->pb_work_dir_browse->setFocus();
        return;
    }

    if( ! work_dir_info.isWritable())
    {
        QMessageBox::critical(this, tr("Error"), tr("No writable pisido work directory. Please define a writable working directory."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->pb_work_dir_browse->setFocus();
        return;
    }


    if( ui->le_package_name->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No package name. Please define package name."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->le_package_name->setFocus();
        return;
    }

    if( ui->le_version->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No version. Please define version."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->le_version->setFocus();
        return;
    }

    if( ui->combo_licence->currentText().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No license selected. Please select a license."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->combo_licence->setFocus();
        return;
    }

    if( ui->combo_is_a->currentText().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No IsA selected. Please select a IsA."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->combo_is_a->setFocus();
        return;
    }

    if( ui->le_brief->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No summary. Please enter summary."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->le_brief->setFocus();
        return;
    }

    if( ui->te_detailed->toPlainText().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No Description. Please enter description."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->te_detailed->setFocus();
        return;
    }

    if( ui->le_packager_name->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No packager name. Please enter packager name."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_packager));
        ui->le_packager_name->setFocus();
        return;
    }

    if( ui->le_packager_email->text().isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No packager email. Please enter packager email."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_packager));
        ui->le_packager_email->setFocus();
        return;
    }

    QString package_name = ui->le_package_name->text();
    QDir work_dir(ui->le_work_dir->text());
    QDir package_dir;

    if((package_dir = get_package_dir(work_dir, package_name)) == QDir())
    {
        return;
    }

    // call creation functions

    if( ! create_pspec_xml(package_dir))
        return;

    if( ! create_action_py(package_dir))
        return;

    if(ui->chk_create_desktop->isChecked())
        if( ! create_desktop(package_dir))
            return;

    if(ui->chk_build->isChecked())
    {
        build_package(package_dir, work_dir);
    }
    else
        QMessageBox::information(this, tr("Success"),
                                 tr("PISI build files successfully created."));
}


bool MainWindow::create_pspec_xml(QDir package_dir)
{
    // define source
    QString src_path;
    bool local_file = true;
    if(ui->rb_src_compressed->isChecked())
    {
        src_path = ui->le_src_compressed->text();
        if( src_path.isEmpty() || ! QFile::exists(src_path))
        {
            QMessageBox::critical(this, tr("Error"), tr("Source not defined !"));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_src_code));
            ui->tb_src_compressed->setFocus();
            return false;
        }
    }
    else if(ui->rb_src_url->isChecked())
    {
        src_path = ui->le_src_url->text();
        if(src_path.isEmpty())
        {
            QMessageBox::critical(this, tr("Error"), tr("Empty URL !"));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_src_code));
            ui->le_src_url->setFocus();
            return false;
        }
        local_file = false;
        if(ui->le_src_sha1->text().isEmpty() || ui->le_src_sha1->text().length() < 40)
        {
            QMessageBox::critical(this, tr("Error"), tr("Fill SHA1 value."));
            return false;
        }
    }
    else if (ui->rb_src_folder)
    {
        src_path = ui->le_src_folder->text();
        if( src_path.isEmpty() || ! QFile::exists(src_path))
        {
            QMessageBox::critical(this, tr("Error"), tr("Source not defined !"));
            ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_src_code));
            ui->tb_src_folder->setFocus();
            return false;
        }
        // compress and copy to work dir
        QDir src_path_dir(src_path);
        QString dir_name = src_path_dir.dirName();
        QDir work_dir = package_dir;
        work_dir.cdUp();
        src_path = work_dir.absoluteFilePath(QString("%1.tar.gz").arg(dir_name));
        src_path_dir.cdUp();
        QProcess tar;
        QStringList parameters;
        parameters << "-zcf" << src_path;
        parameters << "--directory" << src_path_dir.absolutePath() << dir_name;
        tar.start("tar", parameters);
        statusBar()->showMessage(tr("Compressing directory ......"));
        bool ok = false;
        settings.beginGroup( "configuration" );
        int limit = settings.value("folder_comp_time_limit").toInt(&ok);
        settings.endGroup();
        if(!ok) limit = 1;
        if (!tar.waitForFinished(limit*60*1000))        // 5 minutes
        {
            QMessageBox::critical(this, tr("Error"), tr("Source folder could not compress."));
            return false;
        }
        statusBar()->showMessage(tr("Directory compression finished."));
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Source not selected."));
        return false;
    }

    // compute sha1sum
    QString sha1sum;
    if(local_file)
    {
        QFile compressed_file(src_path);
        if( ! compressed_file.open(QIODevice::ReadOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not open file to sha1sum."));
            return false;
        }
        QByteArray compressed_file_bytes = compressed_file.readAll();
        sha1sum = QCryptographicHash::hash(compressed_file_bytes, QCryptographicHash::Sha1).toHex();
        compressed_file_bytes.clear();
        compressed_file.close();
    }
    else
    {
        sha1sum = ui->le_src_sha1->text();
    }


    //copy archive to pisi archive dir
    if(ui->chk_cp_to_pisi_archive->isChecked())
    {
        copy_source_archive(src_path);
    }


    // collect other data
    QString package_name = ui->le_package_name->text();
    QString src_home_page = ui->le_src_home_page->text();
    QString is_a = ui->combo_is_a->currentText();
    QString part_of = ui->combo_part_of->currentText();
    QString license = ui->combo_licence->currentText();
    QString brief = ui->le_brief->text();
    QString detailed = ui->te_detailed->toPlainText();
    QString archive_name;
    if(local_file)
        archive_name = QFileInfo(src_path).fileName();
    else
        archive_name = src_path;
    QString src_file_type = get_archive_type(src_path);
    int release = 1;
    QString release_date = QDate::currentDate().toString("yyyy-MM-dd");
    QString release_version = ui->le_version->text();
    QString release_comment = tr("First Release");
    QString packager_name = ui->le_packager_name->text();
    QString packager_email = ui->le_packager_email->text();
    QStringList build_dependencies = ui->le_build_dependency->text().split(",", QString::SkipEmptyParts);
    QStringList runtime_dependencies = ui->le_runtime_dependency->text().split(",", QString::SkipEmptyParts);


    // prepare xml
    QDomDocument domdom("PISI SYSTEM \"http://www.pardus.org.tr/projeler/pisi/pisi-spec.dtd\"");

    QDomElement pisi = domdom.createElement("PISI");
    domdom.appendChild(pisi);

    QDomElement source = domdom.createElement("Source");
    pisi.appendChild(source);

        QDomElement source_name = domdom.createElement("Name");
        source_name.appendChild(domdom.createTextNode(package_name));
        source.appendChild(source_name);

        QDomElement source_home_page = domdom.createElement("Homepage");
        source_home_page.appendChild(domdom.createTextNode(src_home_page));
        source.appendChild(source_home_page);

        QDomElement source_packager = domdom.createElement("Packager");
        QDomElement source_packager_name = domdom.createElement("Name");
        QDomElement source_packager_email = domdom.createElement("Email");
        source_packager_name.appendChild(domdom.createTextNode(packager_name));
        source_packager_email.appendChild(domdom.createTextNode(packager_email));
        source_packager.appendChild(source_packager_name);
        source_packager.appendChild(source_packager_email);
        source.appendChild(source_packager);

        QDomElement source_is_a = domdom.createElement("IsA");
        source_is_a.appendChild(domdom.createTextNode(is_a));
        source.appendChild(source_is_a);

        QDomElement source_partof = domdom.createElement("PartOf");
        source_partof.appendChild(domdom.createTextNode(part_of));
        source.appendChild(source_partof);

        QDomElement source_license = domdom.createElement("License");
        source_license.appendChild(domdom.createTextNode(license));
        source.appendChild(source_license);

        QDomElement source_summary = domdom.createElement("Summary");
        source_summary.appendChild(domdom.createTextNode(brief));
        source.appendChild(source_summary);

        QDomComment source_summary_comment = domdom.createComment("<Summary xml:lang=\"tr\">Summary in Turkish </Summary>");
        source.insertAfter(source_summary_comment, source_summary);

        QDomElement source_description = domdom.createElement("Description");
        source_description.appendChild(domdom.createTextNode(detailed));
        source.appendChild(source_description);

        QDomComment source_description_comment = domdom.createComment("<Description xml:lang=\"tr\">Description in Turkish</Description>");
        source.insertAfter(source_description_comment, source_description);

        QDomElement source_archive = domdom.createElement("Archive");
        source_archive.setAttribute("type", src_file_type);
        source_archive.setAttribute("sha1sum", sha1sum);
        source_archive.appendChild(domdom.createTextNode(archive_name));
        source.appendChild(source_archive);

        QDomElement source_build_dependencies = domdom.createElement("BuildDependencies");
        source.appendChild(source_build_dependencies);

            foreach (QString dependency, build_dependencies)
            {
                QDomElement source_build_dependency = domdom.createElement("Dependency");
                source_build_dependency.appendChild(domdom.createTextNode(dependency.trimmed()));
                source_build_dependencies.appendChild(source_build_dependency);
            }

        QDomComment source_dependency_patch_comment = domdom.createComment(
                    "\n"
                    "<Patches>\n"
                    "    <Patch>Build path, if any</Patch>\n"
                    "</Patches>"
                    "\n"
                    );
        source.insertAfter(source_dependency_patch_comment, source_archive);


    QDomElement package = domdom.createElement("Package");
    pisi.appendChild(package);

        QDomElement pkg_name = domdom.createElement("Name");
        pkg_name.appendChild(domdom.createTextNode(package_name));
        package.appendChild(pkg_name);


        QDomElement package_runtime_dependencies = domdom.createElement("RuntimeDependencies");
        package.appendChild(package_runtime_dependencies);

            foreach (QString dependency, runtime_dependencies)
            {
                QDomElement package_runtime_dependency = domdom.createElement("Dependency");
                package_runtime_dependency.appendChild(domdom.createTextNode(dependency.trimmed()));
                package_runtime_dependencies.appendChild(package_runtime_dependency);
            }

        QDomElement package_files = domdom.createElement("Files");
        package.appendChild(package_files);

            QDomElement pkg_files_path = domdom.createElement("Path");
            pkg_files_path.setAttribute("fileType", "All");
            pkg_files_path.appendChild(domdom.createTextNode("/"));
            package_files.appendChild(pkg_files_path);

            QDomComment pkg_files_comment = domdom.createComment(
                        "\n"
                        "<Path fileType=\"executable\">/usr/bin</Path>\n"
                        "<Path fileType=\"config\">/etc</Path>\n"
                        "<Path fileType=\"library\">/usr/lib</Path>\n"
                        "<Path fileType=\"header\">/usr/include</Path>\n"
                        "<Path fileType=\"data\">/usr/share</Path>\n"
                        "<Path fileType=\"localedata\">/usr/share/locale</Path>\n"
                        "<Path fileType=\"man\">/usr/share/man</Path>\n"
                        "<Path fileType=\"info\">/usr/share/info</Path>\n"
                        "<Path fileType=\"doc\">/usr/share/doc</Path>\n"
                        "\n"
                        );
            package_files.appendChild(pkg_files_comment);

        if(ui->chk_create_desktop->isChecked())
        {
            QDomElement package_additional_files = domdom.createElement("AdditionalFiles");
            package.appendChild(package_additional_files);

                QDomElement package_ad_file_desktop = domdom.createElement("AdditionalFile");
                package_ad_file_desktop.setAttribute("owner", "root");
                package_ad_file_desktop.setAttribute("permission", "0644");
                package_ad_file_desktop.setAttribute("target", QString("/usr/share/applications/%1.desktop").arg(package_name));
                package_ad_file_desktop.appendChild(domdom.createTextNode(QString("%1.desktop").arg(package_name)));
                package_additional_files.appendChild(package_ad_file_desktop);

                QDomElement package_ad_file_image = domdom.createElement("AdditionalFile");
                package_ad_file_image.setAttribute("owner", "root");
                package_ad_file_image.setAttribute("permission", "0644");
                package_ad_file_image.setAttribute("target", QString("/usr/share/pixmaps/%1.png").arg(package_name));
                package_ad_file_image.appendChild(domdom.createTextNode(QString("%1.png").arg(package_name)));
                package_additional_files.appendChild(package_ad_file_image);
        }
        else
        {
            QDomComment package_additional_files_comment = domdom.createComment(
                        "\n"
                        "<AdditionalFiles>\n"
                            "<AdditionalFile owner=\"root\" permission=\"0644\" target=\"/usr/share/applications/package_name.desktop\">package_name.desktop</AdditionalFile>\n"
                            "<AdditionalFile owner=\"root\" permission=\"0644\" target=\"/usr/share/pixmaps/package_name.png\">package_name.png</AdditionalFile>\n"
                        "</AdditionalFiles>\n"
                        "\n"
                        );
            package.appendChild(package_additional_files_comment);
        }

        QDomComment package_provides_comment = domdom.createComment(
                    "\n"
                    "<Provides>\n"
                        "<COMAR script=\"package.py\">System.Package</COMAR>\n"
                        "<COMAR script=\"service.py\">System.Service</COMAR>\n"
                    "</Provides>\n"
                    "\n"
                    );
        package.appendChild(package_provides_comment);

    QDomElement history = domdom.createElement("History");
    pisi.appendChild(history);

        QDomElement history_update = domdom.createElement("Update");
        history_update.setAttribute("release", QString::number(release));
        history.appendChild(history_update);

            QDomElement history_update_date = domdom.createElement("Date");
            history_update_date.appendChild(domdom.createTextNode(release_date));
            history_update.appendChild(history_update_date);

            QDomElement history_update_version = domdom.createElement("Version");
            history_update_version.appendChild(domdom.createTextNode(release_version));
            history_update.appendChild(history_update_version);

            QDomElement history_update_comment = domdom.createElement("Comment");
            history_update_comment.appendChild(domdom.createTextNode(release_comment));
            history_update.appendChild(history_update_comment);

            QDomElement history_update_name = domdom.createElement("Name");
            history_update_name.appendChild(domdom.createTextNode(packager_name));
            history_update.appendChild(history_update_name);

            QDomElement history_update_email = domdom.createElement("Email");
            history_update_email.appendChild(domdom.createTextNode(packager_email));
            history_update.appendChild(history_update_email);



    // write processed text to file
    QString file_name = package_dir.absoluteFilePath("pspec.xml");
    QFile file(file_name);

    if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(file_name));
        return false;
    }

    QTextStream writer(&file);

    writer << domdom.toString(4);

    return true;
}

bool MainWindow::create_action_py(QDir package_dir)
{
    QString file_name = package_dir.absoluteFilePath("actions.py");
    QFile file(file_name);
    if( ! file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(file_name));
        return false;
    }

    QTextStream writer(&file);

    QTextEdit * te = ui->sw_action_template->currentWidget()->findChild<QTextEdit*>();

    if(te)
    {
        QString action_py = te->toPlainText();
        action_py.replace(QString("___package_name___"), ui->le_package_name->text());
        action_py.replace(QString("___version___"), ui->le_version->text());
        action_py.replace(QString("___summary___"), ui->le_brief->text());

        writer << action_py;
        return true;
    }
    else
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not find object !"));
        return false;
    }
}

bool MainWindow::create_desktop(QDir package_dir)
{
    QString files = "files";
    if( ! package_dir.cd(files) )
    {
        if( ! package_dir.mkdir(files) )
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not create files dir."));
            return false;
        }
        package_dir.cd(files);
    }

    QString package_name = ui->le_package_name->text();
    QString file_name = package_dir.absoluteFilePath(package_name + ".desktop");
    QFile file(file_name);
    if( ! file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(file_name));
        return false;
    }

    QTextStream writer(&file);

    QString desktop_str = ui->pte_desktop->toPlainText();
    desktop_str.replace(QString("___package_name___"), package_name);
    desktop_str.replace(QString("___version___"), ui->le_version->text());
    desktop_str.replace(QString("___summary___"), ui->le_brief->text());

    writer << desktop_str;

    // write image file

    QString image_path = package_dir.absoluteFilePath(package_name + ".png");

    if( ! QFile::exists(image_path))
    {
        // do not remove file; user may changed the file !
        QFile image_file(image_path);
        if( ! image_file.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(package_name + ".png"));
            return false;
        }

        QImage image(":/images/pardus-logo.png");
        image.save(&image_file);
    }

    return true;
}





void MainWindow::on_action_Open_Actions_API_Page_triggered()
{
    settings.beginGroup("configuration");
    QString action_api_page = settings.value("action_api_page").toString();
    if( ! action_api_page.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(action_api_page));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Spec_File_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_spec = settings.value("pisi_spec").toString();
    if( ! pisi_spec.isEmpty())
    {
        QDesktopServices::openUrl(QUrl(pisi_spec));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Archive_Dir_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_archive_dir = settings.value("pisi_archive_dir").toString();
    if( ! pisi_archive_dir.isEmpty())
    {
        QDir dir(pisi_archive_dir);
        if(dir.exists())
            QDesktopServices::openUrl(QUrl(pisi_archive_dir));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Archive_Dir_As_Root_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_archive_dir = settings.value("pisi_archive_dir").toString();
    QString cmd = QString("/usr/bin/xdg-su -u root -c \"dolphin %1\"").arg(pisi_archive_dir);
    if(system(qPrintable(cmd)))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open pisi archive directory !"));
    }
    settings.endGroup();
}

void MainWindow::on_action_Build_Package_triggered()
{
    QString work_dir_str = ui->le_work_dir->text();
    QFileInfo work_dir_info(work_dir_str);
    QString package_name(ui->le_package_name->text());

    if(work_dir_str.isEmpty() || ! work_dir_info.exists())
    {
        QMessageBox::critical(this, tr("Error"), tr("No pisido work directory. Please define a working directory."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->pb_work_dir_browse->setFocus();
        return;
    }
    if( package_name.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("No package name. Please define package name."));
        ui->tabWidget->setCurrentIndex(ui->tabWidget->indexOf(ui->tab_package));
        ui->le_package_name->setFocus();
        return;
    }

    QDir work_dir(work_dir_str);
    QDir package_dir;
    if((package_dir = get_package_dir(work_dir, package_name)) == QDir())
    {
        return;
    }

    if(ui->chk_cp_to_pisi_archive->isChecked())
    {
        QString src_path;
        if(ui->rb_src_compressed->isChecked())
        {
            src_path = ui->le_src_compressed->text();
            if( ! QFile::exists(src_path))
                src_path.clear();
        }
        else if(ui->rb_src_folder->isChecked())
        {
            if( ! ui->le_src_folder->text().isEmpty())
            {
                QDir src_path_dir(ui->le_src_folder->text());
                if(src_path_dir.exists())
                {
                    QString dir_name = src_path_dir.dirName();
                    if( ! dir_name.isEmpty())
                    {
                        // do not compress folder, copy if you can find !
                        QDir work_dir = package_dir;
                        work_dir.cdUp();
                        QString file_path = work_dir.absoluteFilePath(QString("%1.tar.gz").arg(dir_name));
                        if(QFile::exists(file_path))
                            src_path = file_path;
                    }
                }
            }
        }
        else if(ui->rb_src_url->isChecked())
        {
            // no copy, pisi will download
        }

        if( ! src_path.isEmpty())
            copy_source_archive(src_path);
    }

    build_package(package_dir, work_dir);
}


bool MainWindow::build_package(QDir package_dir, QDir out_dir)
{
    if(package_dir == QDir() || out_dir == QDir())
        return false;



    QString cmd = QString("/usr/bin/xdg-su -u root -c \"konsole --noclose --workdir %1 -e pisi bi ./pspec.xml --debug --output-dir %2\"")
            .arg(package_dir.absolutePath())
            .arg(out_dir.absolutePath());

    if(system(qPrintable(cmd)))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not build pisi files !"
                                                    "\nPlease check pisi console output."));
        return false;
    }
    else
    {
        return true;
    }
}


bool MainWindow::copy_source_archive(QString src_path)
{
    settings.beginGroup( "configuration" );
    QString pisi_dir = settings.value("pisi_archive_dir").toString();
    settings.endGroup();
    QString cmd = QString("/usr/bin/xdg-su -u root -c \"/bin/cp %1 %2\"").arg(src_path).arg(pisi_dir);
    if(system(qPrintable(cmd)))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not copy source archive to pisi archive directory !"
                                                    "\nProgress will continue without copying."));
    }
}

QMap<QString, QVariant> MainWindow::get_settings_group(QString group)
{
    QMap<QString, QVariant> map;

    settings.beginGroup(group);
    foreach (QString key, settings.childKeys())
    {
        map[key] = settings.value(key);
    }
    settings.endGroup();
    return map;
}

void MainWindow::on_action_Save_Package_Information_triggered()
{
    QString file_name = QFileDialog::getSaveFileName(this, tr("Save Package Info"),
                                     QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                     tr("PiSiDo Package Info File (*.pisido)"));
    if(file_name.isEmpty())
        return;

    write_settings();

    QMap< int, QMap<QString, QVariant> > map;
    map[0] = get_settings_group("source");
    map[1] = get_settings_group("package");
    map[2] = get_settings_group("compilation");
    map[3] = get_settings_group("packager");

    quint32 version = 0x00000001;

    QFile file(file_name);
    if( ! file.open(QFile::WriteOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open file for writing !"));
        return;
    }
    QDataStream out(&file);
    out << version;
    out << map;
}

void MainWindow::on_action_Load_Package_Information_triggered()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open Package Info"),
                                    QDesktopServices::storageLocation(QDesktopServices::HomeLocation),
                                    tr("PiSiDo Package Info File (*.pisido)"));
    if(file_name.isEmpty())
        return;

    QFile file(file_name);
    if( ! file.open(QFile::ReadOnly))
    {
        QMessageBox::critical(this, tr("Error"), tr("Can not open file for reading !"));
        return;
    }
    QDataStream in(&file);

    QMap< int, QMap<QString, QVariant> > map;
    quint32 version;

    in >> version;

    if(version != 0x00000001)
    {
        QMessageBox::critical(this, tr("Error"), tr("Wrong file version !"));
        return;
    }

    in >> map;

    set_settings_group(map[0], "source");
    set_settings_group(map[1], "package");
    set_settings_group(map[2], "compilation");
    set_settings_group(map[3], "packager");

    read_settings();
}

void MainWindow::set_settings_group(QMap<QString, QVariant> map, QString group)
{
    settings.beginGroup(group);
    QMap<QString, QVariant>::const_iterator i = map.constBegin();
    while(i != map.constEnd())
    {
        settings.setValue(i.key(), i.value());
        ++i;
    }
    settings.endGroup();
}

void MainWindow::on_le_package_name_textChanged(const QString & text)
{
    if(ui->le_work_dir->text().isEmpty() || ui->le_package_name->text().isEmpty())
    {
        ui->pb_import_package->setEnabled(false);
        return;
    }
    QFileInfo fi(ui->le_work_dir->text() +
                 QDir::separator() +
                 ui->le_package_name->text() +
                 QDir::separator() +
                 "pspec.xml"
                 );
    if(fi.exists())
        ui->pb_import_package->setEnabled(true);
    else
        ui->pb_import_package->setEnabled(false);
}

void MainWindow::on_tw_history_itemSelectionChanged()
{
    ui->pb_delete_update->setEnabled( ui->tw_history->selectionModel()->hasSelection());
}

void MainWindow::on_pb_import_package_clicked()
{
    QString file_name = ui->le_work_dir->text() +
                 QDir::separator() +
                 ui->le_package_name->text() +
                 QDir::separator() +
                 "pspec.xml";
    if( ! file_name.isEmpty() && QFile::exists(file_name))
    {
        QFile file(file_name);
        if( ! file.open(QFile::ReadOnly))
        {
            QMessageBox::critical(this, tr("Error"), tr("Can not open file for reading !"));
            return;
        }
        dom_pspec.clear();
        QString errorMsg;
        int errorLine, errorColumn;
        if( ! dom_pspec.setContent(&file, &errorMsg, &errorLine, &errorColumn))
        {
            file.close();
            dom_pspec.clear();
            QMessageBox::critical(this, tr("Parse Error"),
                                  tr("XML Parse Error : \n%1\nLine:%2, Column:%3")
                                    .arg(errorMsg).arg(errorLine).arg(errorColumn)
                                  );
            return;
        }
        file.close();
        fill_fields_from_pspec_xml();
    }
}

void MainWindow::fill_fields_from_pspec_xml()
{
    QDomElement dom_root = dom_pspec.documentElement();
    QDomNodeList list = dom_root.elementsByTagName("Name");
    for(int i=0; i<list.count(); ++i)
    {
        QDomElement e = list.at(i).toElement();
        if( ! e.isNull()) {
            qDebug() << qPrintable(e.tagName()) << endl;
        }
    }
}

