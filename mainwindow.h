#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_action_About_triggered();

    void on_actionAbout_Qt_triggered();

    void on_tb_src_compressed_clicked();

    void on_tb_src_folder_clicked();

    void on_combo_licence_currentIndexChanged(int index);

    void on_tb_work_dir_clicked();

    void on_pb_add_file_clicked();

    void on_pb_remove_file_clicked();

    void on_pb_clear_files_clicked();

    void on_actionConfigure_Application_triggered();

    void on_actionOpen_Help_Page_triggered();

    void on_pb_create_clicked();

private:
    Ui::MainWindow *ui;
    QMap<QString, QString> package_files;

    enum User_Selection_Mode {Folder, File};
    QString get_user_selection( User_Selection_Mode selection_mode, QString setting_group, QString setting_key,
                               QWidget * parent = 0, QString title = QString(), QString file_filter = QString());
};

#endif // MAINWINDOW_H
