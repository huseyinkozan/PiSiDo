#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QSettings>
#include <QDomDocument>

#include "pisi.h"

namespace Ui {
    class MainWindow;
}
class QDir;
class HelpDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_Save_Package_Information_triggered();
    void on_action_Load_Package_Information_triggered();

    void on_action_Open_PISI_Archive_Dir_triggered();
    void on_action_Open_PISI_Archive_Dir_As_Root_triggered();

    void on_action_Configure_Application_triggered();

    void on_action_Help_triggered();
    void on_action_Open_PISI_Spec_File_triggered();
    void on_action_Open_Actions_API_Page_triggered();
    void on_action_About_triggered();
    void on_action_About_Qt_triggered();

    void on_tb_work_dir_browse_clicked();
    void on_pb_work_dir_open_clicked();
    void on_le_work_dir_textChanged(const QString &arg1);
    void on_le_package_name_textChanged(const QString &arg1);
    void on_le_package_name_returnPressed();

    void on_tb_src_compressed_clicked();
    void on_tb_src_folder_clicked();

    void on_tb_desktop_reset_clicked();
    void on_pb_import_package_clicked();
    void on_pb_delete_last_update_clicked();
    void on_pb_add_update_clicked();

    void on_action_Clear_triggered();
    void on_action_Create_triggered();
    void on_action_Build_triggered();
    void on_action_Build_Only_triggered();


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

    QStringList get_file_strings(const QString & file_name);

    enum User_Selection_Mode {Folder, File};
    QString get_user_selection( User_Selection_Mode selection_mode, QString setting_group, QString setting_key,
                               QWidget * parent = 0, QString title = QString(), QString file_filter = QString());
    void write_settings();
    void read_settings();

    QMap<QString, QVariant> get_settings_group(QString group);
    void set_settings_group(QMap<QString, QVariant> map, QString group);

    QString get_sha1sum(QString file_name);
    QString get_archive_type(const QString&);
    QString get_compressed_archive_path(QDir archive_directory);
    QDir get_package_dir(QDir work_dir, QString package_name);

    bool create_pspec_xml(QDir package_dir);
    bool create_action_py(QDir package_dir);
    bool create_desktop(QDir package_dir);

    bool build_package(QDir package_dir, QDir out_dir);
    void copy_source_archive(QString src_path);

    void fill_fields_from_pisi();
    void fill_pisi_from_fields();
};

#endif // MAINWINDOW_H
