#ifndef WORKSPACEDIALOG_H
#define WORKSPACEDIALOG_H

#include <QDialog>

namespace Ui {
    class WorkspaceDialog;
}

class WorkspaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WorkspaceDialog(QWidget *parent = 0);
    ~WorkspaceDialog();

    QString get_workspace();

private slots:
    void save_settings();
    void read_settings();

    void on_pb_browse_clicked();
    void on_le_workspace_textChanged(const QString &arg1);

private:
    Ui::WorkspaceDialog *ui;
    QString workspace;
};

#endif // WORKSPACEDIALOG_H
