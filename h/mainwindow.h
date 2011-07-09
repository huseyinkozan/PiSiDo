#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QMultiMap>
#include <QSettings>
#include <QDomDocument>
#include <QDir>

#include "pisi.h"

namespace Ui {
    class MainWindow;
}

class HelpDialog;
class QsciScintilla;
class QsciLexerPython;
class QFileSystemWatcher;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void save_actions_editor_change();
    void complete_word();

    void package_files_changed();
    void package_files_process(const QString & dir);
    void package_install_changed();

    void clear_tableW_patches();
    void clear_tableW_aditional_files();

    void on_action_Change_Workspace_triggered();

    void on_action_Open_PISI_Archive_Dir_triggered();
    void on_action_Open_PISI_Archive_Dir_As_Root_triggered();

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

    void on_combo_source_currentIndexChanged(int index);
    void on_tb_source_clicked();

    void on_le_package_name_textChanged(const QString &text);
    void on_le_package_name_returnPressed();
    void on_tb_import_package_clicked();

    void on_le_source_textChanged(const QString & text);
    void on_le_source_sha1_editingFinished();

    void on_combo_license_currentIndexChanged(const QString & text);
    void on_combo_is_a_currentIndexChanged(const QString & text);
    void on_combo_part_of_currentIndexChanged(const QString &  text);

    void on_le_homepage_textChanged(const QString & text);
    void on_le_summary_textChanged(const QString & text);
    void on_te_description_textChanged();
    void on_le_build_dependency_textChanged(const QString & text);
    void on_le_runtime_dependency_textChanged(const QString & text);

    void on_action_Reset_Fields_triggered();

    void on_combo_actions_template_currentIndexChanged(int index);
    void on_tb_zoom_in_clicked();
    void on_tb_zoom_out_clicked();

    void on_tb_patch_up_clicked();
    void on_tb_patch_down_clicked();

    void on_tb_edit_aditional_files_clicked();

    void on_tb_open_patches_clicked();

    void on_tb_open_aditional_files_clicked();

    void on_tb_open_install_dir_clicked();

    void on_tb_open_package_dir_clicked();

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    HelpDialog * help_dialog;
    QString desktop_file_default;
    QSettings settings;
    QMap<int, QString> actions_templates_defaults;
    QMap<int, QString> actions_templates;
    QsciScintilla * actions_editor;
    QsciLexerPython * python_lexer;
    QDomDocument dom_pspec;
    Pisi pisi;

    bool not_ask_workspace;

    int selected_source;

    QDir workspace;
    QDir package_dir;
    QDir package_files_dir;
    QDir package_install_dir;

    QFileSystemWatcher * package_files_watcher;
    QFileSystemWatcher * package_install_watcher;

    QString package_name;
    QString source;
    QString source_sha1;
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

    void appy_default_settings();
    void write_settings();
    void read_settings();

    QStringList get_file_strings(const QString & file_name);
    QString get_file_contents(const QString & file_name);
    QString get_sha1sum(const QString & file_name);
    QString get_compressed_archive(QDir dir_to_compress, QDir out_dir);

    PisiUpdate get_last_history_update();
    PisiUpdate get_history_update(int row);

    bool create_action_py(QDir package_dir);
    bool create_desktop(QDir package_dir);

    bool build_package(QDir package_dir, QDir out_dir);

    void fill_fields_from_pisi();
    void fill_pisi_from_fields();

};

#endif // MAINWINDOW_H
