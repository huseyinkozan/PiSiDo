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

#include <qtermwidget/qtermwidget.h>

#include "aboutdialog.h"
#include "addinstallfilelabeldialog.h"
#include "addupdatedialog.h"
#include "aditionalfiledialog.h"
#include "archiveselectiondialog.h"
#include "configurationdialog.h"
#include "helpdialog.h"
#include "languagedialog.h"
#include "workspacedialog.h"

#define DEFAULT_PATCH_LEVEL 1
#define PACKAGE_NAME_REFRESH_INTERVAL 2000

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
    a_dw_actions->setStatusTip(ui->dw_actions->toolTip());
    a_dw_menu->setStatusTip(ui->dw_menu->toolTip());
    a_dw_install_files->setStatusTip(ui->dw_install_files->toolTip());
    a_dw_aditional_files->setStatusTip(ui->dw_aditional_files->toolTip());
    a_dw_patches->setStatusTip(ui->dw_patches->toolTip());
    a_dw_history->setStatusTip(ui->dw_history->toolTip());
    a_dw_build->setStatusTip(ui->dw_build->toolTip());
    a_tBar_operations->setStatusTip(tr("Show or hide %1").arg(ui->tBar_operations->windowTitle()));
    a_tBar_view->setStatusTip(tr("Show or hide %1").arg(ui->tBar_view->windowTitle()));
    a_tBar_help->setStatusTip(tr("Show or hide %1").arg(ui->tBar_help->windowTitle()));
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

    apply_default_settings();

    actions_templates_defaults.clear();
    actions_templates_defaults[0] = get_file_contents(":/files/actions_template_auto.py");
    actions_templates_defaults[1] = get_file_contents(":/files/actions_template_cmake.py");
    actions_templates_defaults[2] = get_file_contents(":/files/actions_template_kde4.py");
    actions_templates_defaults[3] = get_file_contents(":/files/actions_template_python.py");
    actions_templates_defaults[4] = get_file_contents(":/files/actions_template_qt4.py");
    actions_templates_defaults[5] = get_file_contents(":/files/actions_template_scons.py");
    actions_templates_defaults[6] = "";
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    desktop_file_default = ui->pte_desktop->toPlainText();

    // checks build dir and install dir for file changes

    package_files_watcher = new QFileSystemWatcher(this);
    package_install_watcher = new QFileSystemWatcher(this);
    connect(package_files_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(package_files_changed()));
    connect(package_install_watcher, SIGNAL(directoryChanged(QString)), this, SLOT(package_install_changed()));


    read_settings();

    if( ! not_ask_workspace){
        WorkspaceDialog wd(this);
        if(wd.exec() == QDialog::Accepted){
            workspace_dir = QDir(wd.get_workspace());
            not_ask_workspace = wd.get_not_ask_workspace();
        }
        else
            QTimer::singleShot(0, qApp, SLOT(quit()));
    }

    workspace_dir_timer = new QTimer(this);
    workspace_dir_timer->start(PACKAGE_NAME_REFRESH_INTERVAL);
    connect(workspace_dir_timer, SIGNAL(timeout()), this, SLOT(init_package_name_completer()));

    w_terminal = new QTermWidget(1, ui->dw_build);
    QFont terminal_font = QApplication::font();
    terminal_font.setFamily("Monospace");
    terminal_font.setPointSize(10);
    w_terminal->setTerminalFont(terminal_font);
    w_terminal->setColorScheme(COLOR_SCHEME_GREEN_ON_BLACK);
    w_terminal->setScrollBarPosition(QTermWidget::ScrollBarRight);
    QHBoxLayout * build_layout = qobject_cast<QHBoxLayout *>(ui->dw_build_contents->layout());
    if(build_layout){
        build_layout->insertWidget(0, w_terminal);
        w_terminal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }
    else{
        qDebug() << "Problem with inserting terminal widget !";
    }
    w_terminal->setWorkingDirectory(workspace_dir.absolutePath());
    w_terminal->changeDir(workspace_dir.absolutePath());

    ui->le_package_name->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
 {
    QString exit_cmd = "exit\n";
    w_terminal->sendText(exit_cmd);

     write_settings();
     event->accept();
 }

/** Helper function to load each line from file to QStringList. */

QStringList MainWindow::get_file_strings(const QString & file_name)
{
    QFile file(file_name);
    if( ! file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::critical(this, tr("Error"), tr("Can not open \"%1\" resource !").arg(file_name));
        return QStringList();
    }
    QStringList list;
    QTextStream stream(&file);
    while( ! stream.atEnd()){
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
    if(file.open(QFile::ReadOnly | QFile::Text)){
        contents = file.readAll();
    }
    else{
        qDebug() << "Can not load contents of file: " << file_name;
    }
    return contents;
}

bool MainWindow::save_text_file(const QString &file_name, const QString &data)
{
    QFile file(file_name);
    if( ! file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(file_name));
        return false;
    }
    QTextStream writer(&file);
    writer << data;
    return true;
}

void MainWindow::init_package_name_completer()
{
    QStringList list = workspace_dir.entryList(QDir::Dirs|QDir::NoDotAndDotDot|QDir::NoSymLinks, QDir::Name);
    if(list != workspace_package_names){
        workspace_package_names = list;
        QCompleter * completer = new QCompleter(workspace_package_names, this);
        QCompleter * old_completer = ui->le_package_name->completer();
        ui->le_package_name->setCompleter(completer);
        if(old_completer)
            delete old_completer;
    }
}

void MainWindow::on_action_Change_Workspace_triggered()
{
    WorkspaceDialog wd(this);
    if(wd.exec() == QDialog::Accepted){
        workspace_dir = QDir(wd.get_workspace());
        not_ask_workspace = wd.get_not_ask_workspace();
        w_terminal->setWorkingDirectory(workspace_dir.absolutePath());
        w_terminal->changeDir(workspace_dir.absolutePath());
        on_le_package_name_textChanged(ui->le_package_name->text());
        init_package_name_completer();
    }
}

void MainWindow::on_action_About_triggered()
{
    AboutDialog ad(this);
    ad.exec();
}

void MainWindow::on_action_About_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::on_action_Configure_Application_triggered()
{
    ConfigurationDialog cd(this);
    if(cd.exec() == QDialog::Accepted){
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
    if(file_list.isEmpty()){
        QMessageBox::critical(this, tr("Error"), tr("There are no translation files in : %1").arg(lang_dir.absolutePath()));
        return;
    }
    QMap<QString, QString> lang_map;
    lang_map[""] = QString("");
    foreach (QString file, file_list){
        file.remove(".qm");
        QStringList f = file.split('_');
        f.removeFirst();
        QString loc_abr = f.join("_");
        QLocale loc(loc_abr);
        lang_map[QLocale::languageToString(loc.language())] = loc_abr;
    }

    LanguageDialog ld(lang_map.keys(), this);
    int ret = ld.exec();
    if(ret == QDialog::Accepted){
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
    if( ! action_api_page.isEmpty()){
        QDesktopServices::openUrl(QUrl(action_api_page));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Spec_File_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_spec = settings.value("pisi_spec").toString();
    if( ! pisi_spec.isEmpty()){
        QDesktopServices::openUrl(QUrl(pisi_spec));
    }
    settings.endGroup();
}

void MainWindow::on_action_Open_PISI_Packaging_Dir_triggered()
{
    settings.beginGroup("configuration");
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir").toString();
    if( ! pisi_packaging_dir.isEmpty()){
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
    if(help_dialog == NULL){
        help_dialog = new HelpDialog(this);
        help_dialog->show();
    }
    help_dialog->show();
    help_dialog->raise();
    help_dialog->activateWindow();
}

void MainWindow::apply_default_settings()
{
    // set default settings, needed for first run
    settings.beginGroup( "configuration" );
    QString action_api_page = settings.value("action_api_page", QString("http://tr.pardus-wiki.org/Pardus:ActionsAPI")).toString();
    QString pisi_spec = settings.value("pisi_spec", QString("http://svn.pardus.org.tr/uludag/trunk/pisi/pisi-spec.rng")).toString();
    QString pisi_packaging_dir = settings.value("pisi_packaging_dir", QString("/var/pisi/")).toString();

    settings.setValue("action_api_page", action_api_page);
    settings.setValue("pisi_spec", pisi_spec);
    settings.setValue("pisi_packaging_dir", pisi_packaging_dir);
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
}

void MainWindow::read_settings()
{
    settings.beginGroup("main");
    restoreState(settings.value("window_state").toByteArray());
    restoreGeometry(settings.value("window_geometry").toByteArray());
    workspace_dir = settings.value("workspace").toString();
    not_ask_workspace = settings.value("not_ask_workspace", false).toBool();
    settings.endGroup();
}

void MainWindow::on_action_Reset_Fields_triggered()
{
    ui->le_package_name->clear();
    ui->le_homepage->clear();
    ui->combo_license->setCurrentIndex(0);
    ui->combo_is_a->setCurrentIndex(0);
    ui->combo_part_of->setCurrentIndex(0);
    ui->le_summary->clear();
    ui->pte_description->clear();
    ui->le_build_dependency->clear();
    ui->le_runtime_dependency->clear();

//    These will be cleared after gui change :
//      package_name, homepage, license, is_a, part_of, summary, description, build_dependency, runtime_dependency
//    These will be cleared after le_package_name clear :
//      package_files_watcher, aditional_files, patches

    archives.clear();
    foreach (ArchiveWidget * a_w, archive_widgets) {
        delete_archive(a_w);
    }

    actions_templates.clear();
    actions_templates = actions_templates_defaults;
    actions_editor->setText(actions_templates[ui->combo_actions_template->currentIndex()]);

    on_tb_reset_menu_clicked();
    ui->gb_create_menu->setChecked(false);

    clear_tableW_history();

    clear_tableW_files();
    files.clear();

    pisi.clear();
    dom_pspec.clear();
}

void MainWindow::on_tb_reset_menu_clicked()
{
    ui->pte_desktop->setPlainText(desktop_file_default);
}

PisiUpdate MainWindow::get_history_update(int row) throw(QString)
{
    PisiUpdate update;
    if(ui->tableW_history->itemAt(row, 0) == 0)
        return update;
    bool ok = false;
    int release = ui->tableW_history->item(row,0)->text().toInt(&ok);
    if( ! ok)
        throw tr("Error at conversion release string to integer !");
    update.set_release(release);
    update.set_date(QDate::fromString(ui->tableW_history->item(row, 1)->text(),"dd.MM.yyyy"));
    update.set_version(ui->tableW_history->item(row, 2)->text());
    update.set_comment(ui->tableW_history->item(row, 3)->text());
    update.set_packager_name(ui->tableW_history->item(row, 4)->text());
    update.set_packager_email(ui->tableW_history->item(row, 5)->text());
    return update;
}

void MainWindow::set_history_update(const PisiUpdate &update)
{
    QTableWidgetItem * item_release = new QTableWidgetItem(QString::number(update.get_release()));
    QTableWidgetItem * item_date = new QTableWidgetItem(update.get_date().toString("dd.MM.yyyy"));
    QTableWidgetItem * item_version = new QTableWidgetItem(update.get_version());
    QTableWidgetItem * item_comment = new QTableWidgetItem(update.get_comment());
    QTableWidgetItem * item_name = new QTableWidgetItem(update.get_packager_name());
    QTableWidgetItem * item_email = new QTableWidgetItem(update.get_packager_email());
    int row = 0;
    ui->tableW_history->insertRow(row);
    ui->tableW_history->setItem(row, 0, item_release);
    ui->tableW_history->setItem(row, 1, item_date);
    ui->tableW_history->setItem(row, 2, item_version);
    ui->tableW_history->setItem(row, 3, item_comment);
    ui->tableW_history->setItem(row, 4, item_name);
    ui->tableW_history->setItem(row, 5, item_email);
}

void MainWindow::on_tb_delete_last_update_clicked()
{
    if(ui->tableW_history->rowCount()>0)
        ui->tableW_history->removeRow(0);
}

void MainWindow::on_tb_add_update_clicked()
{
    AddUpdateDialog ud(this);
    if(ud.exec() == QDialog::Accepted){
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

void MainWindow::clear_tableW_history()
{
    int row_count = ui->tableW_history->rowCount();
    for(int i=0; i<row_count; ++i){
        ui->tableW_history->removeRow(0);
    }
}

void MainWindow::on_le_package_name_returnPressed()
{
    on_tb_import_package_clicked();
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

        package_install_dir = QDir::root();

        QStringList p_i_w_dirs = package_install_watcher->directories();
        if( ! p_i_w_dirs.isEmpty())
            package_install_watcher->removePaths(p_i_w_dirs);
    }
    else
    {
        package_name = text.trimmed();
        package_dir = QDir(workspace_dir.absoluteFilePath(package_name));
        package_files_dir = QDir(package_dir.absoluteFilePath("files/"));

        if( ! package_files_dir.isRoot() && package_files_dir.exists()){
            QStringList p_f_w_dirs = package_files_watcher->directories();
            if( ! p_f_w_dirs.contains(package_files_dir.absolutePath()))
                package_files_watcher->addPath(package_files_dir.absolutePath());
        }
        else{
            QStringList p_f_w_dirs = package_files_watcher->directories();
            if( ! p_f_w_dirs.isEmpty())
                package_files_watcher->removePaths(p_f_w_dirs);
        }

        QString pisi_work_dir = "/var/pisi/";
        QStringList p_i_w_dirs = package_install_watcher->directories();
        if( ! p_i_w_dirs.contains(pisi_work_dir))
            package_install_watcher->addPath(pisi_work_dir);

        QString v = pisi.get_last_update().get_version();
        int r = pisi.get_last_update().get_release();
        if( ! v.isEmpty() && r > 0){
            package_install_dir = QDir(QString("/var/pisi/%1-%2-%3/install/").arg(package_name).arg(v).arg(r));
        }
        else{
            package_install_dir = QDir::root();
        }
    }
    // to handle text change event
    package_files_changed();
    package_install_changed();
}



void MainWindow::on_le_homepage_textChanged(const QString & text)
{
    homepage = text.trimmed();
}

void MainWindow::on_le_summary_textChanged(const QString & text)
{
    summary = text.trimmed();
}

void MainWindow::on_le_build_dependency_textChanged(const QString & text)
{
    build_dependency = text.trimmed();
}

void MainWindow::on_le_runtime_dependency_textChanged(const QString & text)
{
    runtime_dependency = text.trimmed();
}

void MainWindow::on_combo_license_currentIndexChanged(const QString & text)
{
    license = text.trimmed();
}

void MainWindow::on_combo_is_a_currentIndexChanged(const QString & text)
{
    is_a = text.trimmed();
}

void MainWindow::on_combo_part_of_currentIndexChanged(const QString & text)
{
    part_of = text.trimmed();
}

void MainWindow::on_pte_description_textChanged()
{
    description = ui->pte_description->toPlainText().trimmed();
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
    }
    else{
        ui->tb_open_package_dir->setEnabled(false);
    }

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
        while(it.hasNext()){
            QString sub = it.next();
            if( ! package_files_watcher->directories().contains(sub))
                package_files_watcher->addPath(sub);

            package_files_process(sub);
        }

        patches = temp_patches;
        temp_patches.clear();
        fill_tableW_patches();

        aditional_files = temp_aditional_files;
        temp_aditional_files.clear();
        fill_tableW_aditional_files();
    }
    else{
        clear_tableW_patches();
        clear_tableW_aditional_files();
        ui->tb_open_aditional_files_dir->setEnabled(false);
        ui->tb_open_patches_dir->setEnabled(false);
    }

    if(QFile::exists(package_dir.absoluteFilePath("pspec.xml"))
            && QFile::exists(package_dir.absoluteFilePath("actions.py"))){
        ui->tb_import_package->setEnabled(true);
    }
    else{
        ui->tb_import_package->setEnabled(false);
    }
}

/** patches must filled before call ! */

void MainWindow::fill_tableW_patches()
{
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
}

/** aditional_files must filled before call ! */
void MainWindow::fill_tableW_aditional_files()
{
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
    if( ! package_install_dir.isRoot() && package_install_dir.exists()){
        QFileSystemModel * model = new QFileSystemModel(this);
        model->setReadOnly(true);
        model->setRootPath(package_install_dir.absolutePath());
        ui->treeV_files->setModel(model);
        ui->treeV_files->setRootIndex(model->index(package_install_dir.absolutePath()));
        ui->treeV_files->expandAll();
        QTimer::singleShot(500, ui->treeV_files, SLOT(expandAll()));
        ui->tb_open_install_dir->setEnabled(true);
    }
    else{
        QAbstractItemModel * old_model = ui->treeV_files->model();
        QFileSystemModel * old_fs_model = qobject_cast<QFileSystemModel *>(old_model);
        if(old_fs_model){
            QStandardItemModel * model = new QStandardItemModel(this);
            ui->treeV_files->setModel(model);
            delete old_model;
        }
        ui->tb_open_install_dir->setEnabled(false);
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
    if(it != patches.end()){
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
    if(it != patches.end()){
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
    if( afd.exec() == QDialog::Accepted ){
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
            QString file_type = aifld.get_file_type();
            append_file(path, file_type, permanent);
            QMap<QString, bool> attr;
            attr[file_type] = permanent;
            files[path] = attr;
        }
    }
}

void MainWindow::append_file(const QString &path, const QString &file_type, bool permanent)
{
    ui->tableW_files->insertRow(0);
    ui->tableW_files->setItem(0, 0, new QTableWidgetItem(path));
    ui->tableW_files->setItem(0, 1, new QTableWidgetItem(file_type));
    ui->tableW_files->setItem(0, 2, new QTableWidgetItem(permanent ? tr("True"):tr("False")));
    ui->tableW_files->sortItems(0);
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
    static QString previous_directory;

    ArchiveSelectionDialog::ArchiveType type = ArchiveSelectionDialog::COMPRESSED;
    if(ui->combo_archive_type->currentIndex() == 0)
        type = ArchiveSelectionDialog::COMPRESSED;
    else if(ui->combo_archive_type->currentIndex() == 1)
        type = ArchiveSelectionDialog::URL;
    else
        Q_ASSERT(false);

    ArchiveSelectionDialog asd(this, type);
    asd.set_previous_directory(previous_directory);

    if(asd.exec() == QDialog::Accepted){
        previous_directory = asd.get_previous_directory();
        append_archive(asd.get_archive(), asd.get_sha1());
        QMap<PisiSource::ArchiveAttr,QString> attr;
        attr[PisiSource::SHA1SUM] = asd.get_sha1();
        attr[PisiSource::TYPE] = PisiSource::get_archive_type(asd.get_archive());
        archives[asd.get_archive()] = attr;
    }
}

void MainWindow::append_archive(const QString &archive, const QString &sha1)
{
    QVBoxLayout * w_layout = qobject_cast<QVBoxLayout *>(ui->w_archive_base->layout());
    if( ! w_layout){
        w_layout = new QVBoxLayout;
        w_layout->setMargin(1);
        w_layout->setSpacing(1);
        ui->w_archive_base->setLayout(w_layout);
    }
    ArchiveWidget * a_w = new ArchiveWidget(ui->w_archive_base, archive, sha1);
    connect(a_w, SIGNAL(delete_me(ArchiveWidget*)), this, SLOT(delete_archive(ArchiveWidget*)));
    archive_widgets.append(a_w);
    w_layout->addWidget(a_w);

}

void MainWindow::delete_archive(ArchiveWidget * a_w)
{
    archives.remove(a_w->get_archive());
    disconnect(a_w, SIGNAL(delete_me(ArchiveWidget*)), this, SLOT(delete_archive(ArchiveWidget*)));
    archive_widgets.removeAt(archive_widgets.indexOf(a_w));
    delete a_w;
}


void MainWindow::on_tb_import_package_clicked()
{
    QString pspec_file = package_dir.absoluteFilePath("pspec.xml");

    if(QFile::exists(pspec_file)){
        QFile file(pspec_file);
        if( ! file.open(QFile::ReadOnly)){
            QMessageBox::critical(this, tr("Error"), tr("Can not open file for reading !"));
            return;
        }
        dom_pspec.clear();
        QString errorMsg;
        int errorLine, errorColumn;
        if( ! dom_pspec.setContent(&file, &errorMsg, &errorLine, &errorColumn)){
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

        try{
            pisi.load_from_dom(dom_pspec);
        }
        catch (QString e){
            QMessageBox::critical(this, tr("Error"), tr("An error occured while parsing xml file : %1").arg(e));
            return;
        }
        catch(...){
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }

        try{
            pisi_to_gui();
        }
        catch (QString e){
            QMessageBox::critical(this, tr("Error"), tr("An error occured while filling fields: %1").arg(e));
            return;
        }
        catch (...){
            QMessageBox::critical(this, tr("Error"), tr("Unknownt exception !"));
            return;
        }

        on_le_package_name_textChanged(ui->le_package_name->text());
    }
}

void MainWindow::pisi_to_gui() throw (QString)
{
    if(pisi.is_empty())
        throw tr("Empty pisi file, import pspec.xml before use !");


    // history section
    clear_tableW_history();
    QMap<int, PisiUpdate> updates = pisi.get_updates();
    QList<int> releases = updates.keys();   // keys are asc ordered
    foreach (int r, releases){
        set_history_update(updates[r]);
    }


    // source section
    PisiSource source = pisi.get_source();
    package_name = source.get_name();
    homepage = source.get_home_page();
    QString license = source.get_license();
    is_a = source.get_is_a();
    part_of = source.get_part_of();
    summary = source.get_summary();
    description = source.get_description();
    build_dependency = source.get_build_dependencies_as_stringlist().join(", ");
    archives = source.get_archives();
    QMap<QString, QMap<PisiSource::PatchAttr,QString> > patches = source.get_patches();
    // assign to gui
    ui->le_homepage->setText(homepage);
    ui->combo_license->setCurrentIndex(ui->combo_license->findText(license));
    ui->combo_is_a->setCurrentIndex(ui->combo_is_a->findText(is_a));
    ui->combo_part_of->setCurrentIndex(ui->combo_part_of->findText(part_of));
    ui->le_summary->setText(summary);
    ui->pte_description->setPlainText(description);
    ui->le_build_dependency->setText(build_dependency);
    QList<QString> archive_list = archives.keys();
    foreach (QString a, archive_list) {
        append_archive(a, archives[a][PisiSource::SHA1SUM]);
    }
    this->patches.clear();
    QList<QString> patch_list = patches.keys();
    foreach (QString patch, patch_list) {
        QMap<PisiSource::PatchAttr,QString> attr = patches.value(patch);
        this->patches.insert(attr.value(PisiSource::LEVEL).toInt(), patch);
    }
    fill_tableW_patches();



    // package section
    PisiPackage package = pisi.get_package();
    if(package_name != package.get_name()){
        qDebug() << package_name << "!=" << package.get_name();
        QMessageBox::warning(this,
                             tr("Warning"),
                             tr("Package name is not same with source name in the pspec.xml file !\n"
                                "Application does not support multiple packages !\n"
                                "Process will stop."));
        return;
    }
    runtime_dependency = package.get_runtime_dependencies_as_stringlist().join(", ");
    ui->le_runtime_dependency->setText(runtime_dependency);
    files = package.get_files_as_string_type();
    QList<QString> files_keys = files.keys();
    foreach (QString path, files_keys) {
        QMap<QString, bool> attr = files.value(path);
        append_file(path, attr.keys().first(), attr.value(attr.keys().first()));
    }
    aditional_files = package.get_aditional_files();
    fill_tableW_aditional_files();


    QString actions_py = package_dir.absoluteFilePath("actions.py");
    if(QFile::exists(actions_py)){
        actions_templates[6] = get_file_contents(actions_py);
        ui->combo_actions_template->setCurrentIndex(6);
    }


    QString desktop_file_name = package_files_dir.absoluteFilePath(QString("%1.desktop").arg(package_name));
    if(QFile::exists(desktop_file_name)){
        QString desktop_file_contents = get_file_contents(desktop_file_name);
        ui->gb_create_menu->setChecked(true);
        ui->pte_desktop->setPlainText(desktop_file_contents);
    }else{
        ui->gb_create_menu->setChecked(false);
    }


    statusBar()->showMessage(tr("Package build information successfully imported."));
}

void MainWindow::pisi_from_gui() throw (QString)
{

    // history section
    QMap<int, PisiUpdate> updates;
    int history_row_count = ui->tableW_history->rowCount();
    if(history_row_count == 0)
        throw tr("Please define an update in history !");
    for(int i=0; i<history_row_count; ++i)
    {
        bool ok = false;
        int release = ui->tableW_history->item(i,0)->text().toInt(&ok);
        if(ok)
            updates[release] = get_history_update(i);
        else
            throw tr("Error at conversion release string to integer !");
    }
    pisi.set_updates(updates);


    // source section
    PisiSource source;
    source.set_name(package_name);
    source.set_home_page(homepage);
    source.set_packager(pisi.get_last_update().get_packager_name(), pisi.get_last_update().get_packager_email());
    source.set_license(license);
    source.set_is_a(is_a);
    source.set_part_of(part_of);
    source.set_summary(summary);
    source.set_description(description);
    source.set_build_dependencies(build_dependency);
    source.set_archives(archives);
    QMap<QString, QMap<PisiSource::PatchAttr, QString> > s_p;
    QMultiMap<int, QString>::const_iterator s_p_it = patches.constBegin();
    while (s_p_it != patches.constEnd()) {
        QMap<PisiSource::PatchAttr, QString> s_p_attr;
        s_p_attr[PisiSource::LEVEL] = s_p_it.key();
        s_p[s_p_it.value()] = s_p_attr;
        ++s_p_it;
    }
    source.set_patches(s_p);
    pisi.set_source(source);


    // package section
    PisiPackage package;
    package.set_name(package_name);
    package.set_runtime_dependencies(runtime_dependency);
    // todo : replace __package_name__ in files !
    // todo : replace __version... vs
    package.set_files(files);
    temp_aditional_files.clear();
    QList<QString> a_f_list = aditional_files.keys();
    foreach (QString a_f, a_f_list) {
        QMap<PisiSPBase::AFileAttr,QString> a_f_attr = aditional_files.value(a_f);
        // todo : replace __version... vs
        a_f.replace("__package_name__", package_name);
        temp_aditional_files[a_f] = a_f_attr;
    }
    package.set_aditional_files(temp_aditional_files);
    temp_aditional_files.clear();
    pisi.set_package(package);
}


bool MainWindow::create_build_files()
{
    // fill pisi
    dom_pspec.clear();
    try{
        pisi_from_gui();
        pisi.save_to_dom(dom_pspec);
    }
    catch (QString e){
        QMessageBox::critical(this, tr("Error"), tr("An error occured while filling pisi from fields :\n %1").arg(e));
        return false;
    }
    catch (...){
        QMessageBox::critical(this, tr("Error"), tr("Unknownt exception while filling pisi from fields !"));
        return false;
    }

    if( ! package_dir.exists()){
        QString package_dir_name = package_dir.dirName();
        if(package_dir_name.isEmpty()){
            QMessageBox::critical(this, tr("Error"), tr("Empty package name !"));
            return false;
        }
        if( ! workspace_dir.mkdir(package_dir_name)){
            QMessageBox::critical(this, tr("Error"), tr("Can not create package directory in workspace !"));
            return false;
        }
        if( ! package_dir.mkdir("files")){
            QMessageBox::critical(this, tr("Error"), tr("Can not create files directory in package directory !"));
            return false;
        }
    }

    QString pspec_file_name = package_dir.absoluteFilePath("pspec.xml");
    save_text_file( pspec_file_name, dom_pspec.toString(4) );


    // create actions
    QString actions_file_name = package_dir.absoluteFilePath("actions.py");
    QString action_py = actions_editor->text();
    if(action_py.isEmpty())
        QMessageBox::information(this, tr("Actions API File"), tr("Actions.py is empty !"));
    action_py.replace(QString("__package_name__"), package_name);
    action_py.replace(QString("__version__"), pisi.get_last_update().get_version());
    action_py.replace(QString("__summary__"), summary);
    save_text_file( actions_file_name, action_py );

    on_le_package_name_textChanged(ui->le_package_name->text());

    return true;
}

void MainWindow::on_tb_build_up_to_clicked()
{
    create_build_files();
    QStringList build_steps;
    build_steps.append("--fetch");
    build_steps.append("--unpack");
    build_steps.append("--setup");
    build_steps.append("--build");
    build_steps.append("--install");
    build_steps.append("--check");
    call_pisi_build_command(build_steps.at(ui->combo_build_up_to->currentIndex()));
}

void MainWindow::on_tb_build_only_clicked()
{
    if(ui->combo_build_only->currentIndex() == 0){
        create_build_files();
    }
    else if(ui->combo_build_only->currentIndex() == 1){
        create_build_files();
        call_pisi_build_command("--package");
    }
}

void MainWindow::on_tb_build_all_clicked()
{
    if(create_build_files())
        call_pisi_build_command();
}

void MainWindow::call_pisi_build_command(const QString &build_step)
{
    QString pspec_file = package_dir.absoluteFilePath("pspec.xml");
    if( ! QFile::exists(pspec_file)){
        QMessageBox::critical(this, tr("Error"), tr("There is no PSPEC file : %1 ").arg(pspec_file));
        return;
    }
    QString command = QString("pkexec --user root pisi build %1 %2 --output-dir %3 \n")
            .arg(pspec_file)
            .arg(build_step)
            .arg(workspace_dir.absolutePath())
            ;
    w_terminal->sendText(command);
}

void MainWindow::on_gb_create_menu_toggled(bool checked)
{
    if(checked){
        // create desktop
        QString desktop_file_name = package_files_dir.absoluteFilePath(package_name + ".desktop");
        QString desktop_str = ui->pte_desktop->toPlainText();
        desktop_str.replace(QString("__package_name__"), package_name);
        desktop_str.replace(QString("__version__"), pisi.get_last_update().get_version());
        desktop_str.replace(QString("__summary__"), summary);
        save_text_file( desktop_file_name, desktop_str );

        // write image file
        QString image_name = package_files_dir.absoluteFilePath(package_name + ".png");
        if( ! QFile::exists(image_name)){
            // do not remove file; user may changed the file !
            QFile image_file(image_name);
            if( ! image_file.open(QIODevice::WriteOnly)){
                QMessageBox::critical(this, tr("Error"), tr("Can not open %1 file to write.").arg(package_name + ".png"));
                return;
            }
            QImage image(":/images/operations.png");
            image.save(&image_file);
        }
    }
    else{
        if(QFile::exists(package_files_dir.absoluteFilePath(package_name + ".desktop")))
            QFile::remove(package_files_dir.absoluteFilePath(package_name + ".desktop"));
        if(QFile::exists(package_files_dir.absoluteFilePath(package_name + ".png")))
            QFile::remove(package_files_dir.absoluteFilePath(package_name + ".png"));
    }
}
