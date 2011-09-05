#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QMultiMap>
#include <QSettings>
#include <QDomDocument>
#include <QDir>

#include "pisi.h"
#include "archivewidget.h"

namespace Ui {
    class MainWindow;
}

class QsciScintilla;
class QsciLexerPython;
class QFileSystemWatcher;
class QTermWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void init_package_name_completer();
    void save_actions_editor_change();
    void complete_word();

    void append_archive(const QString & archive, const QString & sha1);
    void delete_archive(ArchiveWidget *);

    void append_file(const QString & path, const QString & file_type, bool permanent);

    void package_files_changed();
    void package_files_process(const QString & dir);
    void package_install_changed();

    void clear_tableW_files();
    void clear_tableW_patches();
    void clear_tableW_aditional_files();
    void clear_tableW_history();

    void on_action_Change_Workspace_triggered();

    void on_action_Open_PISI_Packaging_Dir_triggered();

    void on_action_Configure_Application_triggered();

    void on_action_Help_triggered();

    void on_action_Open_PISI_Spec_File_triggered();
    void on_action_Open_Actions_API_Page_triggered();
    void on_action_Open_Workspace_triggered();

    void on_action_About_triggered();
    void on_action_About_Qt_triggered();

    void on_tb_reset_menu_clicked();
    void on_tb_delete_last_update_clicked();
    void on_tb_add_update_clicked();

    void on_action_Application_Language_triggered();

    void on_le_package_name_textChanged(const QString &text);
    void on_le_package_name_returnPressed();
    void on_tb_import_package_clicked();

    void on_le_license_textChanged(const QString & text);
    void on_le_is_a_textChanged(const QString & text);
    void on_combo_part_of_currentIndexChanged(const QString &  text);

    void on_le_homepage_textChanged(const QString & text);
    void on_le_summary_textChanged(const QString & text);
    void on_pte_description_textChanged();
    void on_le_build_dependency_textChanged(const QString & text);
    void on_le_runtime_dependency_textChanged(const QString & text);

    void on_action_Reset_Fields_triggered();

    void on_combo_actions_template_currentIndexChanged(int index);
    void on_tb_zoom_in_clicked();
    void on_tb_zoom_out_clicked();

    void on_tb_patch_up_clicked();
    void on_tb_patch_down_clicked();

    void on_tb_edit_aditional_files_clicked();

    void on_tb_open_patches_dir_clicked();

    void on_tb_open_aditional_files_dir_clicked();

    void on_tb_open_install_dir_clicked();

    void on_tb_open_package_dir_clicked();

    void on_tb_add_label_clicked();

    void on_tb_delete_label_clicked();

    void on_tableW_files_itemSelectionChanged();

    void on_tb_add_archive_clicked();

    void on_tb_build_up_to_clicked();

    void on_tb_build_only_clicked();

    void on_tb_build_all_clicked();

    void on_gb_create_menu_toggled(bool checked);


protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    QString desktop_file_default;
    QSettings settings;
    QMap<int, QString> actions_templates_defaults;
    QMap<int, QString> actions_templates;
    QsciScintilla * actions_editor;
    QsciLexerPython * python_lexer;
    QTermWidget * w_terminal;
    QDomDocument dom_pspec;
    Pisi pisi;

    bool not_ask_workspace;

    QDir workspace_dir;
    QDir package_dir;
    QDir package_files_dir;
    QDir package_install_dir;

    QFileSystemWatcher * package_files_watcher;
    QFileSystemWatcher * package_install_watcher;
    QList<ArchiveWidget *> archive_widgets;
    QTimer * workspace_dir_timer;
    QStringList workspace_package_names;

    QString package_name;
    QString homepage;
    QString license;
    QString is_a;
    QString part_of;
    QString summary;
    QString description;
    QString build_dependency;
    QString runtime_dependency;
    QMultiMap<int, QString> patches;
    QMultiMap<int, QString> temp_patches;
    QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > aditional_files;
    QMap<QString, QMap<PisiSPBase::AFileAttr,QString> > temp_aditional_files;
    QMap<QString, QMap<QString, bool> > files;

    void apply_default_settings();
    void write_settings();
    void read_settings();

    QStringList get_file_strings(const QString & file_name);
    QString get_file_contents(const QString & file_name);
    bool save_text_file(const QString & file_name, const QString & data);

    PisiUpdate get_history_update(int row) throw(QString);
    void set_history_update(const PisiUpdate & update);
    void fill_tableW_patches();
    void fill_tableW_aditional_files();

    bool create_build_files();
    void call_pisi_build_command(const QString & build_step = QString());

    void pisi_to_gui() throw (QString);
    void pisi_from_gui() throw (QString);

};

#endif // MAINWINDOW_H
