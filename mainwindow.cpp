#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#include <QCloseEvent>
#include <QDate>
#include <QDesktopServices>
#include <QFileDialog>
#include <QMessageBox>
#include <QProcess>
#include <QUrl>
#include <QCryptographicHash>
#include <QTimer>
#include <QShortcut>
#include <QFileSystemModel>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>

#include "addupdatedialog.h"
#include "configurationdialog.h"
#include "helpdialog.h"
#include "languagedialog.h"
#include "workspacedialog.h"

#define FILE_SYSTEM_TIMEOUT 1000


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    help_dialog(NULL),
    not_ask_workspace(false),
    selected_source(0)
{
    ui->setupUi(this);

    addDockWidget(Qt::RightDockWidgetArea, ui->dw_history);
    // tabify for first run
    tabifyDockWidget(ui->dw_actions, ui->dw_desktop);
    tabifyDockWidget(ui->dw_desktop, ui->dw_files);
    tabifyDockWidget(ui->dw_files, ui->dw_aditional_files);
    tabifyDockWidget(ui->dw_aditional_files, ui->dw_patches);
    tabifyDockWidget(ui->dw_patches, ui->dw_history);
    ui->dw_actions->raise();
    // fill view menu
    ui->menu_View->addAction(ui->dw_actions->toggleViewAction());
    ui->menu_View->addAction(ui->dw_desktop->toggleViewAction());
    ui->menu_View->addAction(ui->dw_files->toggleViewAction());
    ui->menu_View->addAction(ui->dw_aditional_files->toggleViewAction());
    ui->menu_View->addAction(ui->dw_patches->toggleViewAction());
    ui->menu_View->addAction(ui->dw_history->toggleViewAction());
    ui->menu_View->addAction(ui->dw_build->toggleViewAction());
    ui->menu_View->addSeparator();
    ui->menu_View->addAction(ui->tBar_view->toggleViewAction());
    // fill view tool bar
    ui->tBar_view->addAction(ui->dw_actions->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_desktop->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_files->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_aditional_files->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_patches->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_history->toggleViewAction());
    ui->tBar_view->addAction(ui->dw_build->toggleViewAction());

    // initialize scintilla
    actions_editor = new QsciScintilla(ui->dw_actions_contents);
    actions_editor->setIndentationsUseTabs(false);
    actions_editor->setTabWidth(4);
    actions_editor->setMarginLineNumbers(1, true);
    actions_editor->setMarginWidth(1, "12345");
    QFont f = actions_editor->font();
    f.setFamily("monospace");
    actions_editor->setFont(f);
    ui->dw_actions_contents->layout()->addWidget(actions_editor);
    // add python highlight support
    python_lexer = new QsciLexerPython(this);
    python_lexer->setIndentationWarning(QsciLexerPython::Inconsistent);
    actions_editor->setLexer(python_lexer);
    // autocompletion related bussiness
    actions_editor->setAutoCompletionSource(QsciScintilla::AcsDocument);
    actions_editor->setAutoCompletionCaseSensitivity(false);
    actions_editor->setAutoCompletionThreshold(1);
    actions_editor->setAutoCompletionShowSingle(false);
    // manually complete
    QShortcut * sc_auto_comp = new QShortcut(QKeySequence("Ctrl+Space"), this);
    connect(sc_auto_comp, SIGNAL(activated()), this, SLOT(complete_word()));
    // to save editor contents
    connect(actions_editor, SIGNAL(textChanged()), SLOT(save_actions_editor_change()));

    // fill comboboxes
    ui->combo_is_a->addItem("");
    ui->combo_is_a->addItems(get_file_strings(":/files/is_a"));
    ui->combo_license->addItem("");
    ui->combo_license->addItems(get_file_strings(":/files/license"));
    ui->combo_part_of->addItem("");
    ui->combo_part_of->addItems(get_file_strings(":/files/part_of"));

    // set default settings, needed for first run
    settings.beginGroup( "configuration" );
    QString action_api_page = settings.value("action_api_page", QString("http://tr.pardus-wiki.org/Pardus:ActionsAPI")).toString();
    QString pisi_spec = settings.value("pisi_spec", QString("http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng")).toString();
    QString pisi_archive_dir = settings.value("pisi_archive_dir", QString("/var/cache/pisi/archives/")).toString();
    bool ok = false;
    int folder_comp_time_limit = settings.value("folder_comp_time_limit", 2).toInt(&ok);
    if(!ok) folder_comp_time_limit = 2;
    settings.setValue("action_api_page", action_api_page);
    settings.setValue("pisi_spec", pisi_spec);
    settings.setValue("pisi_archive_dir", pisi_archive_dir);
    settings.setValue("folder_comp_time_limit", folder_comp_time_limit);
    settings.endGroup();

    actions_templates_defaults.clear();
    actions_templates_defaults[0] = get_file_contents(":/files/actions_template_auto.py");
    actions_templates_defaults[1] = get_file_contents(":/files/actions_template_cmake.py");
    actions_templates_defaults[2] = get_file_contents(":/files/actions_template_kde4.py");
    actions_templates_defaults[3] = get_file_contents(":/files/actions_template_python.py");
    actions_templates_defaults[4] = get_file_contents(":/files/actions_template_qt4.py");
    actions_templates_defaults[5] = get_file_contents(":/files/actions_template_scons.py");
    actions_templates_defaults[6] = "";

    desktop_file_default = ui->pte_desktop->toPlainText();

    // checks build dir and install dir for file changes
    file_system_timer = new QTimer(this);
    connect(file_system_timer, SIGNAL(timeout()), SLOT(check_build_files()));
    connect(file_system_timer, SIGNAL(timeout()), SLOT(check_install_files()));
    connect(file_system_timer, SIGNAL(timeout()), SLOT(check_aditional_files()));
    connect(file_system_timer, SIGNAL(timeout()), SLOT(check_patches()));

    read_settings();

    if( ! not_ask_workspace)
        on_action_Change_Workspace_triggered();

    // remove color support
    ui->w_console->execute("unalias ls ll dir");
    ui->w_console->execute("alias ll=\"ls -l\"");
    // now, there is a workspace
    ui->w_console->execute(QString("cd %1").arg(workspace.absolutePath()));
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

/**
  Helper function to load each line from file to QStringList.
  */

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

QString MainWindow::get_file_contents(const QString & file_name)
{
    QFile file(file_name);
    QString contents;
    if(file.open(QFile::ReadOnly | QFile::Text))
    {
        contents = file.readAll();
    }
    else
    {
        qDebug() << "Can not load contents of file: " << file_name;
    }
    return contents;
}

void MainWindow::on_action_Change_Workspace_triggered()
{
    WorkspaceDialog wd(this);
    if(wd.exec() == QDialog::Accepted)
    {
        workspace = QDir(wd.get_workspace());
        not_ask_workspace = wd.get_not_ask_workspace();
    }
    else
        QTimer::singleShot(0, qApp, SLOT(quit()));

    // TODO : revise after other actions !
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

void MainWindow::on_action_Configure_Application_triggered()
{
    ConfigurationDialog cd(this);
    if(cd.exec() == QDialog::Accepted)
    {
        // apply settings if needed.
    }
}

void MainWindow::on_action_Application_Language_triggered()
{
    QDir lang_dir(PISIDO_LANG_DIR);
    QStringList file_list = lang_dir.entryList(
                            QStringList() << "*.qm",
                            QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot | QDir::Readable,
                            QDir::Name
                            );
    if(file_list.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("There are no translation files !"));
        return;
    }
    QMap<QString, QString> lang_map;
    foreach (QString file, file_list)
    {
        file.remove(".qm");
        QStringList f = file.split('_');
        f.removeFirst();
        QString loc_abr = f.join("_");
        QLocale loc(loc_abr);
        lang_map[QLocale::languageToString(loc.language())] = loc_abr;
    }

    LanguageDialog ld(lang_map.keys(), this);
    int ret = ld.exec();
    if(ret == QDialog::Accepted)
    {
        QString lang = ld.selectedLanguage();
        QString lang_code = lang_map[lang];
        settings.beginGroup("configuration");
        settings.setValue("language", lang_code);
        settings.endGroup();
    }
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

void MainWindow::on_action_Open_Workspace_triggered()
{
    if(workspace.exists())
        QDesktopServices::openUrl(QUrl("file:///" + workspace.absolutePath()));
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
    settings.beginGroup("main");
    settings.setValue("window_state", saveState());
    settings.setValue("window_geometry", saveGeometry());
    settings.setValue("workspace", workspace.absolutePath());
    settings.setValue("not_ask_workspace", not_ask_workspace);
    settings.endGroup();

    settings.beginGroup("central_widget_contents");
    settings.setValue("package_name", package_name);
    settings.setValue("selected_source", selected_source);
    settings.setValue("source", source);
    settings.setValue("source_sha1", source_sha1);
    settings.setValue("homepage", homepage);
    settings.setValue("summary", summary);
    settings.setValue("description", description);
    settings.setValue("license", license);
    settings.setValue("is_a", is_a);
    settings.setValue("part_of", part_of);
    settings.setValue("build_dependency", build_dependency);
    settings.setValue("runtime_dependency", runtime_dependency);
    settings.endGroup();

    settings.beginGroup("compilation");
    settings.setValue("actions_template_index", ui->combo_actions_template->currentIndex());
    settings.setValue("create_desktop", ui->gb_create_desktop->isChecked());
    settings.setValue("desktop_file", ui->pte_desktop->toPlainText());
    settings.setValue("actions_template_auto", actions_templates[0]);
    settings.setValue("actions_template_cmake", actions_templates[1]);
    settings.setValue("actions_template_kde4", actions_templates[2]);
    settings.setValue("actions_template_python", actions_templates[3]);
    settings.setValue("actions_template_qt4", actions_templates[4]);
    settings.setValue("actions_template_scons", actions_templates[5]);
    settings.setValue("actions_template_imported", actions_templates[6]);
    settings.endGroup();
}

void MainWindow::read_settings()
{
    settings.beginGroup("main");
    restoreState(settings.value("window_state").toByteArray());
    restoreGeometry(settings.value("window_geometry").toByteArray());
    workspace = settings.value("workspace").toString();
    not_ask_workspace = settings.value("not_ask_workspace", false).toBool();
    settings.endGroup();

    settings.beginGroup("central_widget_contents");
    package_name = settings.value("package_name").toString();
    selected_source = settings.value("selected_source", 0).toInt();
    source = settings.value("source").toString();
    source_sha1 = settings.value("source_sha1").toString();
    homepage = settings.value("homepage").toString();
    summary = settings.value("summary").toString();
    description = settings.value("description").toString();
    license = settings.value("license", "").toString();
    is_a = settings.value("is_a", "").toString();
    part_of = settings.value("part_of", "").toString();
    build_dependency = settings.value("build_dependency").toString();
    runtime_dependency = settings.value("runtime_dependency").toString();
    settings.endGroup();

    settings.beginGroup("compilation");
    ui->gb_create_desktop->setChecked(settings.value("create_desktop", false).toBool());
    ui->pte_desktop->setPlainText(settings.value("desktop_file", desktop_file_default).toString());
    int last_index = settings.value("actions_template_index",0).toInt();
    ui->combo_actions_template->setCurrentIndex(last_index);
    actions_templates[0] = settings.value("actions_template_auto", actions_templates_defaults[0]).toString();
    actions_templates[1] = settings.value("actions_template_cmake", actions_templates_defaults[1]).toString();
    actions_templates[2] = settings.value("actions_template_kde4", actions_templates_defaults[2]).toString();
    actions_templates[3] = settings.value("actions_template_python", actions_templates_defaults[3]).toString();
    actions_templates[4] = settings.value("actions_template_qt4", actions_templates_defaults[4]).toString();
    actions_templates[5] = settings.value("actions_template_scons", actions_templates_defaults[5]).toString();
    actions_templates[6] = settings.value("actions_template_imported", actions_templates_defaults[6]).toString();
    actions_editor->setText(actions_templates[last_index]);
    settings.endGroup();

    on_combo_source_currentIndexChanged(selected_source);
    ui->le_source->setText(source);
    ui->le_source_sha1->setText(source_sha1);
    ui->le_homepage->setText(homepage);
    ui->le_package_name->setText(package_name);
    ui->le_summary->setText(summary);
    ui->te_description->setPlainText(description);
    ui->combo_license->setCurrentIndex(ui->combo_license->findText(license));
    ui->combo_is_a->setCurrentIndex(ui->combo_is_a->findText(is_a));
    ui->combo_part_of->setCurrentIndex(ui->combo_part_of->findText(part_of));
    ui->le_build_dependency->setText(build_dependency);
    ui->le_runtime_dependency->setText(runtime_dependency);
}

void MainWindow::on_action_Reset_Fields_triggered()
{
    ui->le_package_name->clear();
    ui->combo_source->setCurrentIndex(0);
    ui->le_source->clear();
    ui->le_source_sha1->clear();
    ui->le_homepage->clear();
    ui->combo_license->setCurrentIndex(0);
    ui->combo_is_a->setCurrentIndex(0);
    ui->combo_part_of->setCurrentIndex(0);
    ui->le_summary->clear();
    ui->te_description->clear();
    ui->le_build_dependency->clear();
    ui->le_runtime_dependency->clear();

    // other values clears after widget changes
    source_sha1.clear();

    actions_templates.clear();
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    on_tb_desktop_reset_clicked();
    ui->gb_create_desktop->setChecked(false);

    int row_count = ui->tw_history->rowCount();
    for(int i=0; i<row_count; ++i)
    {
        ui->tw_history->removeRow(0);
    }
}

void MainWindow::on_tb_desktop_reset_clicked()
{
    ui->pte_desktop->setPlainText(desktop_file_default);
}

QString MainWindow::get_sha1sum(const QString & file_name)
{
    QProcess system;
    system.start("sha1sum", QStringList() << file_name);
    if( ! system.waitForFinished())
    {
        throw QString("sha1sum process timeout within 30 seconds.");
    }
    QString sha1(system.readAll());
    return sha1.split(' ').first();
}

QString MainWindow::get_compressed_archive(QDir dir_to_compress, QDir out_dir)
{
    // TODO : add a messagebox to be able to stop compression, and clean the code

    if( ! dir_to_compress.exists())
        throw QString("Archive directory does not exists !");
    if( ! out_dir.exists())
        throw QString("Working directory does not exists !");

    bool ok = false;
    settings.beginGroup( "configuration" );
    int time_limit = settings.value("folder_comp_time_limit").toInt(&ok);
    settings.endGroup();
    if(!ok || time_limit < 1) time_limit = 1;
    time_limit = time_limit * 60 * 1000;      // x minutes in miliseconds

    QString dir_name = dir_to_compress.dirName();
    dir_to_compress.cdUp();
    QString archive_name = out_dir.absoluteFilePath(QString("%1.tar.gz").arg(dir_name));

    QProcess proc_tar;
    QStringList parameters;
    parameters << "-zcf" << archive_name;                       // save in to this archive file
    parameters << "--directory" << dir_to_compress.absolutePath();  // change tar working dir to this
    parameters << dir_name;                                     // compress this dir
    proc_tar.start("tar", parameters);

    statusBar()->showMessage(tr("Compressing directory ......"));

    if (!proc_tar.waitForFinished(time_limit))
        throw QString("Source folder could not compress.");

    statusBar()->showMessage(tr("Directory compression finished."));
    return archive_name;
}

PisiUpdate MainWindow::get_last_history_update()
{
    int row_count = ui->tw_history->rowCount();
    if(row_count < 1)
        return PisiUpdate();
    int last_release = 0;
    int last_release_row = 0;
    for(int row=0; row<row_count; ++row)
    {
        bool ok = false;
        int current_release = ui->tw_history->item(row,0)->text().toInt(&ok);
        if(ok)
        {
            if(current_release > last_release)
            {
                last_release = current_release;
                last_release_row = row;
            }
        }
        else
        {
            throw QString("Error at conversion release string to integer !");
        }
    }
    if(last_release_row != 0)
    {
        return get_history_update(last_release_row);
    }
    else
    {
        throw QString("Release error at history update !");
    }
}

PisiUpdate MainWindow::get_history_update(int row)
{
    PisiUpdate update;
    if(ui->tw_history->itemAt(row, 0) == 0)
        return update;
    bool ok = false;
    int release = ui->tw_history->item(row,0)->text().toInt(&ok);
    if( ! ok)
        throw QString("Error at conversion release string to integer !");
    update.set_release(release);
    update.set_date(QDate::fromString(ui->tw_history->item(row, 1)->text(),"dd.MM.yyyy"));
    update.set_version(ui->tw_history->item(row, 2)->text());
    update.set_comment(ui->tw_history->item(row, 3)->text());
    update.set_packager_name(ui->tw_history->item(row, 4)->text());
    update.set_packager_email(ui->tw_history->item(row, 5)->text());
    return update;
}

void MainWindow::on_pb_delete_last_update_clicked()
{
    if(ui->tw_history->rowCount()>0)
        ui->tw_history->removeRow(0);
}

void MainWindow::on_pb_add_update_clicked()
{
    AddUpdateDialog ud(this);
    if(ud.exec() == QDialog::Accepted)
    {
        QString release;
        if(ui->tw_history->rowCount() > 0)
            release = QString::number(ui->tw_history->item(0,0)->text().toInt()+1);
        else
            release = "1";
        QTableWidgetItem * item_release = new QTableWidgetItem(release);
        QTableWidgetItem * item_date = new QTableWidgetItem(ud.get_date());
        QTableWidgetItem * item_version = new QTableWidgetItem(ud.get_version());
        QTableWidgetItem * item_comment = new QTableWidgetItem(ud.get_comment());
        QTableWidgetItem * item_name = new QTableWidgetItem(ud.get_packager_name());
        QTableWidgetItem * item_email = new QTableWidgetItem(ud.get_packager_email());
        int row = 0;
        ui->tw_history->insertRow(row);
        ui->tw_history->setItem(row, 0, item_release);
        ui->tw_history->setItem(row, 1, item_date);
        ui->tw_history->setItem(row, 2, item_version);
        ui->tw_history->setItem(row, 3, item_comment);
        ui->tw_history->setItem(row, 4, item_name);
        ui->tw_history->setItem(row, 5, item_email);
    }
}

void MainWindow::on_combo_source_currentIndexChanged(int index)
{
    selected_source = index;
    ui->le_source->clear();
    ui->le_source_sha1->clear();
    if(index == 0)
    {
        ui->le_source_sha1->setVisible(false);
        ui->lbl_source_sha1->setVisible(false);
        ui->tb_source->setVisible(true);
    }
    else if(index == 1)
    {
        ui->le_source_sha1->setVisible(false);
        ui->lbl_source_sha1->setVisible(false);
        ui->tb_source->setVisible(true);
    }
    else if(index == 2)
    {
        ui->le_source_sha1->setVisible(true);
        ui->lbl_source_sha1->setVisible(true);
        ui->tb_source->setVisible(false);
    }
    else
        Q_ASSERT(false);
}

void MainWindow::on_tb_source_clicked()
{
    QString selection;
    int current_index = ui->combo_source->currentIndex();
    if(current_index == 0)
    {
        QString filter = tr("Compressed Files (*.targz *.tarbz2 *.tarlzma *.tarxz *.tarZ *.tar *.zip *.gz *.gzip *.bz2 *.bzip2 *.lzma *.xz *.binary)");
        selection = QFileDialog::getOpenFileName(this, tr("Select Compressed File"), filter);
    }
    else if(current_index == 1)
    {
        QString home_dir = QDesktopServices::storageLocation(QDesktopServices::HomeLocation);
        selection = QFileDialog::getExistingDirectory(this, tr("Select Directory"), home_dir);
    }
    else if(current_index == 2)
    {
        ; // url
    }
    else
        Q_ASSERT(false);

    if(selection.isEmpty())
        return;
    ui->le_source->setText(selection);
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
    QString action_py = actions_editor->text();
    action_py.replace(QString("___package_name___"), ui->le_package_name->text());
    action_py.replace(QString("___version___"), get_last_history_update().get_version());
    action_py.replace(QString("___summary___"), ui->le_summary->text());
    writer << action_py;
    return true;
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
    desktop_str.replace(QString("___version___"), pisi.get_last_update().get_version());
    desktop_str.replace(QString("___summary___"), ui->le_summary->text());

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


/**
  Helper function to call "pisi build" from command line.
  */

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

void MainWindow::on_le_package_name_textChanged(const QString &text)
{
    ui->pb_import_package->setEnabled(false);

    if(text.isEmpty())
    {
        package_name.clear();
        package_dir = QDir();
        package_files_dir = QDir();
        package_install_dir = QDir();
        return;
    }

    package_name = text;
    package_dir = QDir(workspace.absoluteFilePath(package_name));
    package_files_dir = QDir(package_dir.absoluteFilePath("files"));
    package_install_dir= QDir(QString("/var/pisi/%1/install/").arg(package_name));

    if(package_name.isEmpty())
    {
        file_system_timer->stop();
        // to clear operations
        check_build_files();
        check_install_files();
        check_aditional_files();
        check_patches();
    }
    else
    {
        file_system_timer->start(FILE_SYSTEM_TIMEOUT);
    }
}

void MainWindow::on_le_source_textChanged(const QString & text)
{
    source = text;
}

void MainWindow::on_le_source_sha1_editingFinished()
{
    source_sha1 = ui->le_source_sha1->text();
}

void MainWindow::on_le_homepage_textChanged(const QString & text)
{
    homepage = text;
}

void MainWindow::on_le_summary_textChanged(const QString & text)
{
    summary = text;
}

void MainWindow::on_le_build_dependency_textChanged(const QString & text)
{
    build_dependency = text;
}

void MainWindow::on_le_runtime_dependency_textChanged(const QString & text)
{
    runtime_dependency = text;
}

void MainWindow::on_combo_license_currentIndexChanged(const QString & text)
{
    license = text;
}

void MainWindow::on_combo_is_a_currentIndexChanged(const QString & text)
{
    is_a = text;
}

void MainWindow::on_combo_part_of_currentIndexChanged(const QString & text)
{
    part_of = text;
}

void MainWindow::on_te_description_textChanged()
{
    description = ui->te_description->toPlainText();
}

void MainWindow::on_le_package_name_returnPressed()
{
    on_pb_import_package_clicked();
}

void MainWindow::on_pb_import_package_clicked()
{
    QString pspec_file = package_dir.absoluteFilePath("pspec.xml");

    if(QFile::exists(pspec_file))
    {
        QFile file(pspec_file);
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
        pisi.clear();

        try
        {
            pisi.load_from_dom(dom_pspec);
        }
        catch (QString e)
        {
            QMessageBox::critical(this, tr("Error"), tr("An error occured while parsing xml file : %1").arg(e));
            return;
        }
        catch(...)
        {
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }

        try
        {
            fill_fields_from_pisi();
        }
        catch (QString e)
        {
            QMessageBox::critical(this, tr("Error"), tr("An error occured while filling fields: %1").arg(e));
            return;
        }
        catch (...)
        {
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }
    }
}

/**
  Helper function to fill program fields from Pisi class.
  */

void MainWindow::fill_fields_from_pisi()
{
    if(pisi.is_empty())
        throw QString("Try to load fileds from pisi, but pisi not loaded !");

    // source section
    PisiSource source = pisi.get_source();

    QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > archives = source.get_archives();
    // TODO : revise filling comma seperated archives
    if(archives.count() > 1)
    {
        throw QString("More than one archive tag.");
    }
    QMap<QString, QMap<PisiSource::ArchiveAttr,QString> >::const_iterator archives_it = archives.constBegin();
    if(archives_it != archives.constEnd())
    {
        QString archive = archives_it.key();
        if(archive.startsWith("http:") || archive.startsWith("ftp:"))
        {
//            ui->rb_src_url->setChecked(true);
//            ui->le_src_url->setText(archive);
            QMap<PisiSource::ArchiveAttr,QString> archive_att = archives.constBegin().value();
            ui->le_source_sha1->setText(archive_att.value(PisiSource::SHA1SUM));
        }
        else
        {
//            ui->rb_src_compressed->setChecked(true);
//            ui->le_src_compressed->setText(archive);
        }
    }
//    ui->le_src_home_page->setText(source.get_home_page());

    // package section
    PisiPackage package = pisi.get_package();

    // do not edit work_dir and packge_name, only warn
    if(ui->le_package_name->text() != package.get_name())
    {
        qDebug() << ui->le_package_name->text() << "!=" << package.get_name();
        QMessageBox::warning(this, tr("Warning"), tr("Package name is not same as in the pspec.xml file !"));
    }
    ui->le_summary->setText(source.get_summary());
    ui->te_description->setPlainText(source.get_description());
    int license_index = ui->combo_license->findText(source.get_license());
    if(license_index > 0) ui->combo_license->setCurrentIndex(license_index);
    int is_a_index = ui->combo_is_a->findText(source.get_is_a());
    if(is_a_index > 0) ui->combo_is_a->setCurrentIndex(is_a_index);
    int part_of_index = ui->combo_part_of->findText(source.get_part_of());
    if(part_of_index > 0) ui->combo_part_of->setCurrentIndex(part_of_index);

    QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > build_dep = source.get_build_dependencies();
    ui->le_build_dependency->setText(source.get_dependency_list(build_dep).join(", "));

    QMap<QString, QMap<PisiSPBase::VRTFAttr,QString> > runtime_dep = package.get_runtime_dependencies();
    ui->le_runtime_dependency->setText(package.get_dependency_list(runtime_dep).join(", "));

    QString actions_py = package_dir.absoluteFilePath("actions.py");

    if(QFile::exists(actions_py))
    {
        actions_templates[6] = get_file_contents(actions_py);
    }
    ui->combo_actions_template->setCurrentIndex(6);

    QString desktop_file_name = package_files_dir.absoluteFilePath(QString("%1.desktop").arg(package_name));

    if(QFile::exists(desktop_file_name))
    {
        QFile desktop_file(desktop_file_name);
        if( ! desktop_file.open(QFile::ReadOnly | QFile::Text))
        {
            QMessageBox::warning(this, tr("Error"), tr("Can not open desktop file for reading !"));
        }
        else
        {
            ui->gb_create_desktop->setChecked(true);
            QTextStream desktop_file_stream(&desktop_file);
            ui->pte_desktop->setPlainText(desktop_file_stream.readAll());
        }
        desktop_file.close();
    }
    else
    {
        ui->gb_create_desktop->setChecked(false);
    }

    // history section
    int row_count = ui->tw_history->rowCount();
    for(int i=0; i<row_count; ++i)
    {
        ui->tw_history->removeRow(0);
    }

    QMap<int, PisiUpdate> updates = pisi.get_updates();
    QList<int> releases = updates.keys();   // keys are asc ordered
    foreach (int r, releases)
    {
        QTableWidgetItem * item_release = new QTableWidgetItem(QString::number(updates[r].get_release()));
        QTableWidgetItem * item_date = new QTableWidgetItem(updates[r].get_date().toString("dd.MM.yyyy"));
        QTableWidgetItem * item_version = new QTableWidgetItem(updates[r].get_version());
        QTableWidgetItem * item_comment = new QTableWidgetItem(updates[r].get_comment());
        QTableWidgetItem * item_name = new QTableWidgetItem(updates[r].get_packager_name());
        QTableWidgetItem * item_email = new QTableWidgetItem(updates[r].get_packager_email());
        int row = 0;
        ui->tw_history->insertRow(row);
        ui->tw_history->setItem(row, 0, item_release);
        ui->tw_history->setItem(row, 1, item_date);
        ui->tw_history->setItem(row, 2, item_version);
        ui->tw_history->setItem(row, 3, item_comment);
        ui->tw_history->setItem(row, 4, item_name);
        ui->tw_history->setItem(row, 5, item_email);
    }

    // packager section
    // packager will not import

    statusBar()->showMessage(tr("Package build information successfully imported."));
}

void MainWindow::fill_pisi_from_fields()
{
    if(pisi.is_empty())
        throw QString("Try to load pisi from fileds, but pisi not loaded !");

    // source section
    PisiSource source;

    QString package_name = ui->le_package_name->text().trimmed();
    if(package_name.isEmpty())
        throw QString("Please select a package name !");
    source.set_name(package_name);

    if(homepage.isEmpty())
        throw QString("Please enter homepage !");
    source.set_name(homepage);

    PisiUpdate last_update = get_last_history_update();
    if(last_update == PisiUpdate())
        throw QString("Please define an update in history !");
    QMap<QString,QString> packager;
    packager[last_update.get_packager_name()] = last_update.get_packager_email();
    source.set_packager(packager);

    QString license = ui->combo_license->currentText().trimmed();
    if(license.isEmpty())
        throw QString("Please select a license !");
    source.set_license(license);

    QString is_a = ui->combo_is_a->currentText().trimmed();
    if( ! is_a.isEmpty())
        source.set_is_a(is_a);

    QString part_of = ui->combo_part_of->currentText().trimmed();
    if( ! part_of.isEmpty())
        source.set_part_of(part_of);

    QString summary = ui->le_summary->text().trimmed();
    if(summary.isEmpty())
        throw QString("Please enter summary !");
    source.set_summary(summary);

    QString description = ui->te_description->toPlainText().trimmed();
    if( ! description.isEmpty())
        source.set_description(description);

    QMap<QString, QMap<PisiSource::ArchiveAttr,QString> > archives = source.get_archives();
    // TODO : revise filling comma seperated archives
    if(archives.count() > 1)
    {
        throw QString("More than one archive tag.");
    }
    archives.clear();
//    if(ui->rb_src_compressed->isChecked())
//    {
//        QString a = ui->le_src_compressed->text();
//        if(a.isEmpty() || ! QFile::exists(a))
//            throw QString("Please select a compressed source !");
//        QMap<PisiSource::ArchiveAttr,QString> attr;
//        attr[PisiSource::SHA1SUM] = PisiSource::get_sha1sum(a);
//        attr[PisiSource::TYPE] = PisiSource::get_archive_type(a);
//        archives[a] = attr;
//    }
//    else if(ui->rb_src_folder->isChecked())
//    {
//        QString f = ui->le_src_folder->text();
//        if(f.isEmpty() || ! QDir(f).exists())
//            throw QString("Please select a source folder !");
//        QString w = ui->le_work_dir->text();
//        if(w.isEmpty() || ! QDir(w).exists())
//            throw QString("Please select a work dir !");
//        QString a = get_compressed_archive(f, w);
//        QMap<PisiSource::ArchiveAttr,QString> attr;
//        attr[PisiSource::SHA1SUM] = PisiSource::get_sha1sum(a);
//        attr[PisiSource::TYPE] = PisiSource::get_archive_type(a);
//        archives[a] = attr;
//    }
//    else if(ui->rb_src_url->isChecked())
//    {
//        QString u = ui->le_src_url->text();
//        if(u.isEmpty() || ! (u.startsWith("http:") || u.startsWith("ftp:")))
//            throw QString("Please enter a proper url (starts with http: or ftp:) !");
//        QString s1 = ui->le_source_sha1->text();
//        if(s1.isEmpty() || s1.length() < 40)
//            throw QString("Please enter a proper sha1sum value !");
//        QMap<PisiSource::ArchiveAttr,QString> attr;
//        attr[PisiSource::SHA1SUM] = s1;
//        attr[PisiSource::TYPE] = PisiSource::get_archive_type(u);
//        archives[u] = attr;
//    }
//    source.set_archives(archives);

    QString b_dep_str = ui->le_build_dependency->text().trimmed();
    if( ! b_dep_str.isEmpty())
        source.set_build_dependencies(b_dep_str);

    // package section
    PisiPackage package;
    package.set_name(package_name);
    package.set_license(license);
    if( ! is_a.isEmpty())
        package.set_is_a(is_a);
    if( ! part_of.isEmpty())
        package.set_part_of(part_of);
    package.set_summary(summary);
    if( ! description.isEmpty())
        package.set_description(description);
    QString r_dep_str = ui->le_runtime_dependency->text().trimmed();
    if( ! r_dep_str.isEmpty())
        package.set_runtime_dependencies(r_dep_str);

    QMap<QString, QMap<PisiPackage::FileType, bool> > files;
    QMap<PisiPackage::FileType, bool> files_attribute;
    files_attribute.insert(PisiPackage::ALL, false);
    files.insert("/", files_attribute);
    package.set_files(files);

    // history section
    QMap<int, PisiUpdate> updates;
    int history_row_count = ui->tw_history->rowCount();
    for(int i=0; i<history_row_count; ++i)
    {
        bool ok = false;
        int release = ui->tw_history->item(i,0)->text().toInt(&ok);
        if(ok)
            updates[release] = get_history_update(i);
        else
            throw QString("Error at conversion release string to integer !");
    }

    // if not exception threw
    pisi.set_source(source);
    pisi.set_package(package);
    pisi.set_updates(updates);
}


void MainWindow::on_combo_actions_template_currentIndexChanged(int index)
{
    actions_editor->setText(actions_templates[index]);
}

void MainWindow::save_actions_editor_change()
{
    int current_index = ui->combo_actions_template->currentIndex();
    actions_templates[current_index] = actions_editor->text();
}

void MainWindow::on_tb_zoom_in_clicked()
{
    actions_editor->zoomIn();
}

void MainWindow::on_tb_zoom_out_clicked()
{
    actions_editor->zoomOut();
}

void MainWindow::complete_word()
{
    if(actions_editor->hasFocus())
        actions_editor->autoCompleteFromDocument();
}

void MainWindow::check_build_files()
{
    if(QFile::exists(package_dir.absoluteFilePath("pspec.xml"))
            && QFile::exists(package_dir.absoluteFilePath("actions.py")))
    {
        ui->pb_import_package->setEnabled(true);
    }
    else
    {
        ui->pb_import_package->setEnabled(false);
    }
}

void MainWindow::check_install_files()
{
    // TODO :
    if(package_install_dir.exists()){
        // ckeck files
        // fill widgets
        // enable package only
    }
    else{
        // clear widgets
        // disable package only
    }

}

void MainWindow::check_aditional_files()
{
    // TODO :
    if(package_files_dir.exists()){
        // ckeck aditional files
        // fill widgets
    }
    else{
        // clear widgets
    }
}

void MainWindow::check_patches()
{
    // TODO :
    if(package_files_dir.exists()){
        // ckeck patches
        // fill widgets
    }
    else{
        // clear widgets
    }
}




