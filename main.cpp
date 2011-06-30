#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTranslator>
#include <QLocale>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // set Application information
    a.setOrganizationName(QString("pisido"));
    a.setOrganizationDomain(QString("huseyinkozan.com.tr"));
    a.setApplicationName(QString("pisido"));
    a.setApplicationVersion(QString("%1").arg(PISIDO_VERSION));

    // do translation bussiness
    QSettings settings;
    settings.beginGroup("configuration");
    QString lang_code = settings.value("language", QString()).toString();
    settings.endGroup();
    if(lang_code.isEmpty())
        lang_code = QLocale::system().name();
    QTranslator translator;
    QDir lang_dir(PISIDO_LANG_DIR);
    QString lang_file = lang_dir.absoluteFilePath(QString("pisido_%1.qm").arg(lang_code));
    if( ! QFile::exists(lang_file)){
        lang_file.replace(lang_code, QLocale().system().name());
    }
    if(QFile::exists(lang_file)){
        if(translator.load(lang_file)){
            a.installTranslator(&translator);
        }
        else{
            qDebug() << QObject::tr("Translator failed to load : ") << lang_file;
        }
    }
    else{
        qDebug() << QObject::tr("Translator file does not exists : ") << lang_file;
    }


    MainWindow w;
    w.show();

    return a.exec();
}
