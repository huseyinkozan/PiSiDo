#include "workspacedialog.h"
#include "ui_workspacedialog.h"

#include <QSettings>
#include <QFileDialog>

WorkspaceDialog::WorkspaceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WorkspaceDialog)
{
    ui->setupUi(this);

    connect(this, SIGNAL(accepted()), SLOT(save_settings()));

    read_settings();
}

WorkspaceDialog::~WorkspaceDialog()
{
    delete ui;
}

void WorkspaceDialog::on_pb_browse_clicked()
{
    QString new_workspace = QFileDialog::getExistingDirectory(this, tr("Select Workspace"), workspace);
    if(new_workspace.isEmpty())
        return;
    workspace = new_workspace;
    ui->le_workspace->setText(workspace);
}

void WorkspaceDialog::on_le_workspace_textChanged(const QString &arg1)
{
    if(QDir(workspace).exists())
    {
        ui->check_not_ask->setEnabled(true);
        ui->pb_ok->setEnabled(true);
    }
    else
    {
        ui->check_not_ask->setEnabled(false);
        ui->pb_ok->setEnabled(false);
    }
}

void WorkspaceDialog::save_settings()
{
    if(workspace.isEmpty())
        return;
    QSettings settings;
    settings.beginGroup("main");
    settings.setValue("not_ask_workspace", ui->check_not_ask->isChecked());
    settings.setValue("workspace", workspace);
    settings.endGroup();
}

void WorkspaceDialog::read_settings()
{
    QSettings settings;
    settings.beginGroup("main");
    workspace = settings.value("workspace").toString();
    settings.endGroup();
    ui->le_workspace->setText(workspace);
}

QString WorkspaceDialog::get_workspace()
{
    return workspace;
}
