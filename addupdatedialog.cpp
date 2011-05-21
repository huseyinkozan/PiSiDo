#include "addupdatedialog.h"
#include "ui_addupdatedialog.h"

AddUpdateDialog::AddUpdateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddUpdateDialog)
{
    ui->setupUi(this);
}

AddUpdateDialog::~AddUpdateDialog()
{
    delete ui;
}
