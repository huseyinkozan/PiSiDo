#include "aboutdialog.h"
#include "ui_aboutdialog.h"

#include "directorymodel.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);

    ui->lbl_program_name->setText(qApp->applicationName());
    ui->lbl_program_version->setText(qApp->applicationVersion());

    ui->treeView_test->setModel(new DirectoryModel(QDir(qApp->applicationDirPath()), this));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}
