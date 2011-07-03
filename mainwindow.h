#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include <QDomDocument>
#include <QDir>

#include "pisi.h"

namespace Ui {
    class MainWindow;
}
class QFile;
class HelpDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
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

    void on_tb_desktop_reset_clicked();
    void on_pb_delete_last_update_clicked();
    void on_pb_add_update_clicked();

    void on_action_Clear_triggered();
    void on_action_Create_triggered();
    void on_action_Build_triggered();
    void on_action_Build_Only_triggered();
    void on_action_Application_Language_triggered();

    void on_combo_source_currentIndexChanged(int index);
    void on_tb_source_clicked();

    void on_le_package_name_textChanged(const QString &text);
    void on_le_package_name_returnPressed();
    void on_pb_import_package_clicked();

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

protected:
    virtual void closeEvent(QCloseEvent * event);

private:
    Ui::MainWindow *ui;
    HelpDialog * help_dialog;
    QStringList action_defaults;
    QString desktop_file_default;
    QSettings settings;
    QDomDocument dom_pspec;
    Pisi pisi;

    bool not_ask_workspace;

    int selected_source;

    QDir workspace;
    QDir package_dir;
    QDir package_files_dir;

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

    void write_settings();
    void read_settings();

    QStringList get_file_strings(const QString & file_name);
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
