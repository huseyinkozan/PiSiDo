#include "application.h"
#include <QDir>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

#include "mainwindow.h"

void load_translation(const QApplication & a);



int main(int argc, char *argv[])
{
    Application a(argc, argv);

    // set Application information
    a.setOrganizationName(QString("pisido"));
    a.setOrganizationDomain(QString("huseyinkozan.com.tr"));
    a.setApplicationName(QString("PiSiDo"));
    a.setApplicationVersion(QString("%1").arg(PISIDO_VERSION));

    load_translation(a);

    MainWindow w;
    w.show();

    return a.exec();
}



void load_translation(const QApplication & a)
{
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
//        qDebug() << QObject::tr("Translator file does not exists : ") << lang_file;
    }
}
