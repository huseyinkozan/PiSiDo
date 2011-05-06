#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set Application information
    a.setOrganizationName(QString("pisido"));
    a.setOrganizationDomain(QString("huseyinkozan.com.tr"));
    a.setApplicationName(QString("pisido"));
    a.setApplicationVersion(QString("%1").arg(PISIDO_VERSION));

    // do translation bussiness
    QTranslator translator;
    QString locale = QLocale::system().name();
    translator.load(QString(":/translations/pisido_%1.qm").arg(locale));
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}
