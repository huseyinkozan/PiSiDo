#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName(QString("huseyinkozan"));
    a.setOrganizationDomain(QString("huseyinkozan.com.tr"));
    a.setApplicationName(QString("pisido"));
    a.setApplicationVersion(QString("%1").arg(PISIDO_VERSION));
    QTranslator translator;
    translator.load(":/pisido_tr");
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
