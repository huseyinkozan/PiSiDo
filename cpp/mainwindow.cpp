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
#include <QFileSystemWatcher>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QDirIterator>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerpython.h>

#include "addinstallfilelabeldialog.h"
#include "addupdatedialog.h"
#include "aditionalfiledialog.h"
#include "archiveselectiondialog.h"
#include "configurationdialog.h"
#include "helpdialog.h"
#include "languagedialog.h"
#include "workspacedialog.h"

#define DEFAULT_PATCH_LEVEL 1

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    help_dialog(NULL),
    not_ask_workspace(false),
    workspace_dir(QDir::root()),
    package_dir(QDir::root()),
    package_files_dir(QDir::root()),
    package_install_dir(QDir::root())
{
    ui->setupUi(this);

    addDockWidget(Qt::RightDockWidgetArea, ui->dw_history);
    // tabify for first run
    tabifyDockWidget(ui->dw_actions, ui->dw_menu);
    tabifyDockWidget(ui->dw_menu, ui->dw_install_files);
    tabifyDockWidget(ui->dw_install_files, ui->dw_aditional_files);
    tabifyDockWidget(ui->dw_aditional_files, ui->dw_patches);
    tabifyDockWidget(ui->dw_patches, ui->dw_history);
    ui->dw_actions->raise();
    // fill view menu
    QAction * a_dw_actions = ui->dw_actions->toggleViewAction();
    QAction * a_dw_menu = ui->dw_menu->toggleViewAction();
    QAction * a_dw_install_files = ui->dw_install_files->toggleViewAction();
    QAction * a_dw_aditional_files = ui->dw_aditional_files->toggleViewAction();
    QAction * a_dw_patches = ui->dw_patches->toggleViewAction();
    QAction * a_dw_history = ui->dw_history->toggleViewAction();
    QAction * a_dw_build = ui->dw_build->toggleViewAction();
    QAction * a_tBar_operations = ui->tBar_operations->toggleViewAction();
    QAction * a_tBar_view = ui->tBar_view->toggleViewAction();
    QAction * a_tBar_help = ui->tBar_help->toggleViewAction();
    a_dw_actions->setIcon(QIcon(":/images/actions.png"));
    a_dw_menu->setIcon(QIcon(":/images/menu.png"));
    a_dw_install_files->setIcon(QIcon(":/images/install-files.png"));
    a_dw_aditional_files->setIcon(QIcon(":/images/aditional-files.png"));
    a_dw_patches->setIcon(QIcon(":/images/patches.png"));
    a_dw_history->setIcon(QIcon(":/images/history.png"));
    a_dw_build->setIcon(QIcon(":/images/build.png"));
    a_tBar_operations->setIcon(QIcon(":/images/operations.png"));
    a_tBar_view->setIcon(QIcon(":/images/toolbar.png"));
    a_tBar_help->setIcon(QIcon(":/images/help.png"));
    ui->menu_View->addAction(a_dw_actions);
    ui->menu_View->addAction(a_dw_menu);
    ui->menu_View->addAction(a_dw_install_files);
    ui->menu_View->addAction(a_dw_aditional_files);
    ui->menu_View->addAction(a_dw_patches);
    ui->menu_View->addAction(a_dw_history);
    ui->menu_View->addAction(a_dw_build);
    ui->menu_View->addSeparator();
    ui->menu_View->addAction(a_tBar_operations);
    ui->menu_View->addAction(a_tBar_view);
    ui->menu_View->addAction(a_tBar_help);
    // fill view tool bar
    ui->tBar_view->addAction(a_dw_actions);
    ui->tBar_view->addAction(a_dw_menu);
    ui->tBar_view->addAction(a_dw_install_files);
    ui->tBar_view->addAction(a_dw_aditional_files);
    ui->tBar_view->addAction(a_dw_patches);
    ui->tBar_view->addAction(a_dw_history);
    ui->tBar_view->addAction(a_dw_build);

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

    appy_default_settings();

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

    package_files_watcher = new QFileSystemWatcher(this);
//    package_install_watcher = new QFileSystemWatcher(this);
    connect(package_files_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(package_files_changed()));
//    connect(package_install_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(package_install_changed()));


    read_settings();

    if( ! not_ask_workspace)
    {
        WorkspaceDialog wd(this);
        if(wd.exec() == QDialog::Accepted)
        {
            workspace_dir = QDir(wd.get_workspace());
            not_ask_workspace = wd.get_not_ask_workspace();
        }
        else
            QTimer::singleShot(0, qApp, SLOT(quit()));
    }

    // remove color support
    ui->w_console->execute("unalias ls ll dir");
    ui->w_console->execute("alias ll=\"ls -l\"");
    // now, there is a workspace
    ui->w_console->execute(QString("cd %1").arg(workspace_dir.absolutePath()));
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
        workspace_dir = QDir(wd.get_workspace());
        not_ask_workspace = wd.get_not_ask_workspace();
        ui->w_console->execute(QString("cd %1").arg(workspace_dir.absolutePath()));
    }

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

void MainWindow::on_action_Open_PISI_Packaging_Dir_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir").toString();
    if( ! pisi_packaging_dir.isEmpty())
    {
        QDir dir(pisi_packaging_dir);
        if(dir.exists())
            QDesktopServices::openUrl(QUrl(pisi_packaging_dir));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_Workspace_triggered()
{
    if( ! workspace_dir.isRoot() && workspace_dir.exists())
        QDesktopServices::openUrl(QUrl("file:///" + workspace_dir.absolutePath()));
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

void MainWindow::appy_default_settings()
{
    // set default settings, needed for first run
    settings.beginGroup( "configuration" );
    QString action_api_page = settings.value("action_api_page", QString("http://tr.pardus-wiki.org/Pardus:ActionsAPI")).toString();
    QString pisi_spec = settings.value("pisi_spec", QString("http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng")).toString();
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir", QString("/var/pisi/")).toString();
    int console_max_line = settings.value("console_max_line", 100).toInt();

    settings.setValue("action_api_page", action_api_page);
    settings.setValue("pisi_spec", pisi_spec);
    settings.setValue("pisi_packaging_dir", pisi_packaging_dir);

    settings.setValue("console_max_line", console_max_line);
    settings.endGroup();
}

void MainWindow::write_settings()
{
    settings.beginGroup("main");
    settings.setValue("window_state", saveState());
    settings.setValue("window_geometry", saveGeometry());
    settings.setValue("workspace", workspace_dir.absolutePath());
    settings.setValue("not_ask_workspace", not_ask_workspace);
    settings.endGroup();

    settings.beginGroup("central_widget_contents");
    settings.setValue("package_name", package_name);
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
    settings.setValue("create_desktop", ui->gb_create_menu->isChecked());
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
    workspace_dir = settings.value("workspace").toString();
    not_ask_workspace = settings.value("not_ask_workspace", false).toBool();
    settings.endGroup();

    settings.beginGroup("central_widget_contents");
    package_name = settings.value("package_name").toString();
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
    ui->gb_create_menu->setChecked(settings.value("create_desktop", false).toBool());
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
    ui->le_homepage->clear();
    ui->combo_license->setCurrentIndex(0);
    ui->combo_is_a->setCurrentIndex(0);
    ui->combo_part_of->setCurrentIndex(0);
    ui->le_summary->clear();
    ui->te_description->clear();
    ui->le_build_dependency->clear();
    ui->le_runtime_dependency->clear();

    // TODO : clear archive widget

    actions_templates.clear();
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    on_tb_reset_menu_clicked();
    ui->gb_create_menu->setChecked(false);

    int row_count = ui->tableW_history->rowCount();
    for(int i=0; i<row_count; ++i)
    {
        ui->tableW_history->removeRow(0);
    }

    clear_tableW_files();
    files.clear();

    // aditional_files and patches will be cleared by le_package_name change
}

void MainWindow::on_tb_reset_menu_clicked()
{
    ui->pte_desktop->setPlainText(desktop_file_default);
}

PisiUpdate MainWindow::get_last_history_update()
{
    int row_count = ui->tableW_history->rowCount();
    if(row_count < 1)
        return PisiUpdate();
    int last_release = 0;
    int last_release_row = 0;
    for(int row=0; row<row_count; ++row)
    {
        bool ok = false;
        int current_release = ui->tableW_history->item(row,0)->text().toInt(&ok);
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
    if(last_release != 0)
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
    if(ui->tableW_history->itemAt(row, 0) == 0)
        return update;
    bool ok = false;
    int release = ui->tableW_history->item(row,0)->text().toInt(&ok);
    if( ! ok)
        throw QString("Error at conversion release string to integer !");
    update.set_release(release);
    update.set_date(QDate::fromString(ui->tableW_history->item(row, 1)->text(),"dd.MM.yyyy"));
    update.set_version(ui->tableW_history->item(row, 2)->text());
    update.set_comment(ui->tableW_history->item(row, 3)->text());
    update.set_packager_name(ui->tableW_history->item(row, 4)->text());
    update.set_packager_email(ui->tableW_history->item(row, 5)->text());
    return update;
}

void MainWindow::on_tb_delete_last_update_clicked()
{
    if(ui->tableW_history->rowCount()>0)
        ui->tableW_history->removeRow(0);
}

void MainWindow::on_tb_add_update_clicked()
{
    AddUpdateDialog ud(this);
    if(ud.exec() == QDialog::Accepted)
    {
        QString release;
        if(ui->tableW_history->rowCount() > 0)
            release = QString::number(ui->tableW_history->item(0,0)->text().toInt()+1);
        else
            release = "1";
        QTableWidgetItem * item_release = new QTableWidgetItem(release);
        QTableWidgetItem * item_date = new QTableWidgetItem(ud.get_date());
        QTableWidgetItem * item_version = new QTableWidgetItem(ud.get_version());
        QTableWidgetItem * item_comment = new QTableWidgetItem(ud.get_comment());
        QTableWidgetItem * item_name = new QTableWidgetItem(ud.get_packager_name());
        QTableWidgetItem * item_email = new QTableWidgetItem(ud.get_packager_email());
        int row = 0;
        ui->tableW_history->insertRow(row);
        ui->tableW_history->setItem(row, 0, item_release);
        ui->tableW_history->setItem(row, 1, item_date);
        ui->tableW_history->setItem(row, 2, item_version);
        ui->tableW_history->setItem(row, 3, item_comment);
        ui->tableW_history->setItem(row, 4, item_name);
        ui->tableW_history->setItem(row, 5, item_email);
        package_install_changed();
    }
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
    if(package_dir.isRoot() || out_dir.isRoot())
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
    if(text.isEmpty())
    {
        package_name.clear();
        package_dir = QDir::root();
        package_files_dir = QDir::root();

        QStringList p_f_w_dirs = package_files_watcher->directories();
        if( ! p_f_w_dirs.isEmpty())
            package_files_watcher->removePaths(p_f_w_dirs);
//        QStringList p_i_w_dirs = package_install_watcher->directories();
//        if( ! p_i_w_dirs.isEmpty())
//            package_install_watcher->removePaths(p_i_w_dirs);
    }
    else
    {


        package_name = text;
        package_dir = QDir(workspace_dir.absoluteFilePath(package_name));
        package_files_dir = QDir(package_dir.absoluteFilePath("files"));

        if( ! package_files_dir.isRoot() && package_files_dir.exists()){
            package_files_watcher->addPath(package_files_dir.absolutePath());
        }
        else{
            QStringList p_f_w_dirs = package_files_watcher->directories();
            if( ! p_f_w_dirs.isEmpty())
                package_files_watcher->removePaths(p_f_w_dirs);
        }

//        if( ! package_install_dir.isRoot() && package_install_dir.exists()){
//            package_install_watcher->addPath(package_install_dir.absolutePath());
//        }
//        else{
//            QStringList p_i_w_dirs = package_install_watcher->directories();
//            if( ! p_i_w_dirs.isEmpty())
//                package_install_watcher->removePaths(p_i_w_dirs);
//        }
    }
    // to handle text change event
    package_files_changed();
    package_install_changed();
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
    on_tb_import_package_clicked();
}

void MainWindow::on_tb_import_package_clicked()
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
//            ui->le_source_sha1->setText(archive_att.value(PisiSource::SHA1SUM));
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
            ui->gb_create_menu->setChecked(true);
            QTextStream desktop_file_stream(&desktop_file);
            ui->pte_desktop->setPlainText(desktop_file_stream.readAll());
        }
        desktop_file.close();
    }
    else
    {
        ui->gb_create_menu->setChecked(false);
    }

    // history section
    int row_count = ui->tableW_history->rowCount();
    for(int i=0; i<row_count; ++i)
    {
        ui->tableW_history->removeRow(0);
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
        ui->tableW_history->insertRow(row);
        ui->tableW_history->setItem(row, 0, item_release);
        ui->tableW_history->setItem(row, 1, item_date);
        ui->tableW_history->setItem(row, 2, item_version);
        ui->tableW_history->setItem(row, 3, item_comment);
        ui->tableW_history->setItem(row, 4, item_name);
        ui->tableW_history->setItem(row, 5, item_email);
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
    int history_row_count = ui->tableW_history->rowCount();
    for(int i=0; i<history_row_count; ++i)
    {
        bool ok = false;
        int release = ui->tableW_history->item(i,0)->text().toInt(&ok);
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

void MainWindow::package_files_changed()
{
    if( ! package_dir.isRoot() && package_dir.exists())
    {
        ui->tb_open_package_dir->setEnabled(true);

        if( ! package_files_dir.isRoot() && package_files_dir.exists())
        {
            ui->tb_open_aditional_files_dir->setEnabled(true);
            ui->tb_open_patches_dir->setEnabled(true);

            clear_tableW_patches();
            temp_patches.clear();
            clear_tableW_aditional_files();
            temp_aditional_files.clear();

            package_files_process(package_files_dir.absolutePath());

            QDirIterator it(package_files_dir.absolutePath(), QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks|QDir::Readable, QDirIterator::Subdirectories);
            while(it.hasNext())
            {
                QString sub = it.next();
                if( ! package_files_watcher->directories().contains(sub))
                    package_files_watcher->addPath(sub);

                package_files_process(sub);
            }

            patches = temp_patches;
            temp_patches.clear();
            QMultiMap<int, QString>::iterator patch_it = patches.begin();
            int patch_index = 0;
            while(patch_it != patches.end())
            {
                ui->tableW_patches->insertRow(patch_index);
                ui->tableW_patches->setItem(patch_index, 0, new QTableWidgetItem(QString::number(patch_it.key())));
                ui->tableW_patches->setItem(patch_index, 1, new QTableWidgetItem(patch_it.value()));
                patch_index++;
                patch_it++;
            }
            ui->tableW_patches->sortByColumn(0, Qt::AscendingOrder);

            aditional_files = temp_aditional_files;
            temp_aditional_files.clear();
            QStringList a_files_keys = aditional_files.keys();
            int a_file_index = 0;
            foreach (QString a_file, a_files_keys) {
                QMap<PisiSPBase::AFileAttr,QString> attr = aditional_files.value(a_file);
                ui->tableW_aditional_files->insertRow(a_file_index);
                ui->tableW_aditional_files->setItem(a_file_index, 0, new QTableWidgetItem(a_file));
                ui->tableW_aditional_files->setItem(a_file_index, 1, new QTableWidgetItem(attr.value(PisiSPBase::TARGET)));
                ui->tableW_aditional_files->setItem(a_file_index, 2, new QTableWidgetItem(attr.value(PisiSPBase::PERMISSION)));
                ui->tableW_aditional_files->setItem(a_file_index, 3, new QTableWidgetItem(attr.value(PisiSPBase::OWNER)));
                ui->tableW_aditional_files->setItem(a_file_index, 4, new QTableWidgetItem(attr.value(PisiSPBase::GROUP)));
                a_file_index++;
            }
        }
    }

    if( package_dir.isRoot() || ! package_dir.exists())
    {
        ui->tb_open_package_dir->setEnabled(false);
    }

    if(package_files_dir.isRoot() || ! package_files_dir.exists())
    {
        clear_tableW_patches();
        clear_tableW_aditional_files();
        ui->tb_open_aditional_files_dir->setEnabled(false);
        ui->tb_open_patches_dir->setEnabled(false);
    }

    if(QFile::exists(package_dir.absoluteFilePath("pspec.xml"))
            && QFile::exists(package_dir.absoluteFilePath("actions.py")))
    {
        ui->tb_import_package->setEnabled(true);
    }
    else
    {
        ui->tb_import_package->setEnabled(false);
    }
}

void MainWindow::package_files_process(const QString & dir)
{
    QDir sub_dir(dir);
    QFileInfoList files_info_list = sub_dir.entryInfoList(
                QDir::NoDotAndDotDot | QDir::Files | QDir::NoSymLinks | QDir::Readable,
                QDir::Name);

    QStringList patches_values = patches.values();
    QStringList aditional_files_keys = aditional_files.keys();

    foreach (QFileInfo file_info, files_info_list) {
        QString relative_file_path = file_info.absoluteFilePath().remove(package_files_dir.absolutePath() + QDir::separator());
        if(file_info.suffix() == "patch")
        {
            if(patches_values.contains(relative_file_path)){
                temp_patches.insert(patches.key(relative_file_path), relative_file_path);
            }
            else{
                temp_patches.insert(DEFAULT_PATCH_LEVEL, relative_file_path);
            }
        }
        else{
            if(aditional_files_keys.contains(relative_file_path)){
                temp_aditional_files.insert(relative_file_path, aditional_files.value(relative_file_path));
            }
            else{
                QMap<PisiSPBase::AFileAttr,QString> value;
                value.insert(PisiSPBase::TARGET, QString("/usr/share/%1/%2").arg("__package_name__").arg(file_info.fileName()));
                value.insert(PisiSPBase::PERMISSION, QString("0644"));
                value.insert(PisiSPBase::OWNER, QString("root"));
                value.insert(PisiSPBase::GROUP, QString("root"));
                temp_aditional_files.insert(relative_file_path, value);
            }
        }
    }
}

void MainWindow::package_install_changed()
{
    package_install_dir = QDir::root();

    if( ! package_name.isEmpty()){
        PisiUpdate last_update;
        try{
             last_update = get_last_history_update();
        }
        catch (QString e){
            QMessageBox::critical(this, tr("Error"), tr("Cannot get last history update : %1").arg(e));
            return;
        }
        catch(...){
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }
        package_install_dir= QDir(QString("/var/pisi/%1-%2-%3/install/")
                                  .arg(package_name)
                                  .arg(last_update.get_version())
                                  .arg(last_update.get_release()));
    }

    if( ! package_install_dir.isRoot() && package_install_dir.exists()){
        QFileSystemModel * model = new QFileSystemModel(this);
        model->setReadOnly(true);
        model->setRootPath(package_install_dir.absolutePath());
        ui->treeV_files->setModel(model);
        ui->treeV_files->setRootIndex(model->index(package_install_dir.absolutePath()));
        ui->treeV_files->expandAll();
        QTimer::singleShot(500, ui->treeV_files, SLOT(expandAll()));
    }
    else{
        QAbstractItemModel * old_model = ui->treeV_files->model();
        QFileSystemModel * old_fs_model = qobject_cast<QFileSystemModel *>(old_model);
        if(old_fs_model){
            QStandardItemModel * model = new QStandardItemModel(this);
            ui->treeV_files->setModel(model);
            delete old_model;
        }
    }
}

void MainWindow::on_tb_patch_down_clicked()
{
    QModelIndexList list = ui->tableW_patches->selectionModel()->selectedRows(1);
    if(list.count() != 1 )
        return;
    bool ok = false;
    int key = ui->tableW_patches->item(list.first().row(), 0)->data(Qt::DisplayRole).toInt(&ok);
    if( ! ok) key = 1;
    QString value = ui->tableW_patches->item(list.first().row(), 1)->data(Qt::DisplayRole).toString();
    QMultiMap<int, QString>::iterator it = patches.find(key, value);
    if(it != patches.end())
    {
        patches.remove(it.key(), it.value());
        key++;
        patches.insert(key, value);
    }
    package_files_changed();
}

void MainWindow::on_tb_patch_up_clicked()
{
    QModelIndexList list = ui->tableW_patches->selectionModel()->selectedRows(1);
    if(list.count() != 1 )
        return;
    bool ok = false;
    int key = ui->tableW_patches->item(list.first().row(), 0)->data(Qt::DisplayRole).toInt(&ok);
    if( ! ok) key = 1;
    if(key <= 1)
        return;
    QString value = ui->tableW_patches->item(list.first().row(), 1)->data(Qt::DisplayRole).toString();
    QMultiMap<int, QString>::iterator it = patches.find(key, value);
    if(it != patches.end())
    {
        patches.remove(it.key(), it.value());
        key--;
        patches.insert(key, value);
    }
    package_files_changed();
}

void MainWindow::clear_tableW_patches()
{
    int row_count = ui->tableW_patches->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_patches->removeRow(0);
    }
}

void MainWindow::clear_tableW_aditional_files()
{
    int row_count = ui->tableW_aditional_files->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_aditional_files->removeRow(0);
    }
}

void MainWindow::on_tb_edit_aditional_files_clicked()
{
    QModelIndexList list = ui->tableW_aditional_files->selectionModel()->selectedRows(0);
    if(list.count() != 1 )
        return;
    QString a_file = ui->tableW_aditional_files->item(list.first().row(), 0)->data(Qt::DisplayRole).toString();
    AditionalFileDialog afd(this, a_file, aditional_files.value(a_file));
    if( afd.exec() == QDialog::Accepted )
    {
        aditional_files[a_file] = afd.get_attr();
        package_files_changed();
    }
}

void MainWindow::clear_tableW_files()
{
    int row_count = ui->tableW_files->rowCount();
    for (int i = 0; i < row_count; ++i) {
        ui->tableW_files->removeRow(0);
    }
}

void MainWindow::on_tb_add_label_clicked()
{
    QModelIndexList list = ui->treeV_files->selectionModel()->selectedRows(0);
    if(list.count() != 1)
        return;
    QAbstractItemModel * a_model = ui->treeV_files->model();
    QFileSystemModel * model = qobject_cast<QFileSystemModel *>(a_model);
    if(model){
        bool is_dir = model->isDir(list.first());
        QString path = model->filePath(list.first()).remove(package_install_dir.absolutePath());
        if(is_dir)
            path.append("/*");
        AddInstallFileLabelDialog aifld(this, path);
        if(aifld.exec() == QDialog::Accepted){
            bool permanent = aifld.get_permanent();
            ui->tableW_files->insertRow(0);
            ui->tableW_files->setItem(0, 0, new QTableWidgetItem(path));
            ui->tableW_files->setItem(0, 1, new QTableWidgetItem(aifld.get_file_type()));
            ui->tableW_files->setItem(0, 2, new QTableWidgetItem(permanent ? tr("True"):tr("False")));
            ui->tableW_files->sortItems(0);
            QMap<QString, bool> attr;
            attr[aifld.get_file_type()] = aifld.get_permanent();
            files[path] = attr;
        }
    }
}

void MainWindow::on_tb_delete_label_clicked()
{
    QModelIndexList list = ui->tableW_files->selectionModel()->selectedRows(0);
    if(list.count() == 1){
        QString path = ui->tableW_files->item(list.first().row(), 0)->data(Qt::DisplayRole).toString();
        files.remove(path);
        ui->tableW_files->removeRow(list.first().row());
    }
}

void MainWindow::on_tableW_files_itemSelectionChanged()
{
    QModelIndexList list = ui->tableW_files->selectionModel()->selectedRows(0);
    ui->tb_delete_label->setEnabled(list.count() == 1);
}

void MainWindow::on_tb_open_patches_dir_clicked()
{
    on_tb_open_aditional_files_dir_clicked();
}

void MainWindow::on_tb_open_aditional_files_dir_clicked()
{
    if( ! package_files_dir.isRoot() && package_files_dir.exists())
        QDesktopServices::openUrl(QUrl(package_files_dir.absolutePath()));
}

void MainWindow::on_tb_open_install_dir_clicked()
{
    if( ! package_install_dir.isRoot() && package_install_dir.exists())
        QDesktopServices::openUrl(QUrl(package_install_dir.absolutePath()));
}

void MainWindow::on_tb_open_package_dir_clicked()
{
    if( ! package_dir.isRoot() && package_dir.exists())
        QDesktopServices::openUrl(QUrl(package_dir.absolutePath()));
}



void MainWindow::on_tb_add_archive_clicked()
{
    ArchiveSelectionDialog::ArchiveType type;
    if(ui->combo_archive_type->currentIndex() == 0)
        type = ArchiveSelectionDialog::COMPRESSED;
    else if(ui->combo_archive_type->currentIndex() == 1)
        type = ArchiveSelectionDialog::URL;
    else
        Q_ASSERT(false);

    ArchiveSelectionDialog asd(this, type);
    if(asd.exec() == QDialog::Accepted){
        // append archive widget
        QVBoxLayout * w_layout = qobject_cast<QVBoxLayout *>(ui->w_archive_base->layout());
        if( ! w_layout){
            w_layout = new QVBoxLayout;
            ui->w_archive_base->setLayout(w_layout);
        }
        ArchiveWidget * a_w = new ArchiveWidget(ui->w_archive_base, asd.get_archive(), asd.get_sha1());
        connect(a_w, SIGNAL(delete_me(ArchiveWidget*)), SLOT(delete_archive(ArchiveWidget*)));
        archives.append(a_w);
        w_layout->addWidget(a_w);
    }
}

void MainWindow::delete_archive(ArchiveWidget * a_w)
{
    archives.removeAt(archives.indexOf(a_w));
    delete a_w;
}
