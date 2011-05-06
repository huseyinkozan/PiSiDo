#include "helpdialog.h"
#include "ui_helpdialog.h"

#include <QFile>
#include <QDebug>

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    QString parm;
    if(QLocale().system().language() == QLocale::Turkish)
        parm = "_tr";
    ui->textBrowser->setSource(QUrl(QString("qrc:///files/help%1.html").arg(parm)));
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
