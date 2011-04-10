#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_About_triggered()
{
    QMessageBox::about(this, tr("About"), trUtf8("This program written by Hüseyin Kozan."
                                            "\nhttp://huseyinkozan.com.tr"
                                            "\nApp Version:%1").arg(qApp->applicationVersion()));
}

void MainWindow::on_actionAbout_Qt_triggered()
{
    QMessageBox::aboutQt(this);
}
