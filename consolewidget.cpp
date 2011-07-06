#include "consolewidget.h"
#include "ui_consolewidget.h"

#include <QKeyEvent>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QScrollBar>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWidget),
    current_console_line(0)
{
    ui->setupUi(this);

    QSettings settings;
    settings.beginGroup("configuration");
    max_console_line = settings.value("console_max_line", 100).toInt();
    settings.endGroup();

    ui->le_input->installEventFilter(this);

    bash = new QProcess(this);
    bash->setReadChannelMode(QProcess::MergedChannels);
    connect(bash, SIGNAL(readyReadStandardOutput()), SLOT(write_output()));
    bash->start("bash", QStringList() << "-i");
}

ConsoleWidget::~ConsoleWidget()
{
    bash->close();
    delete ui;
}

bool ConsoleWidget::eventFilter(QObject * object, QEvent * event)
{
    if(object == ui->le_input)
    {
        if(event->type() == QEvent::KeyPress)
        {
            QKeyEvent * key_event = static_cast<QKeyEvent *>(event);
            if((key_event->key() == Qt::Key_Return) || (key_event->key() == Qt::Key_Enter))
            {
                execute(ui->le_input->text());
                ui->le_input->clear();
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    else
        return QWidget::eventFilter(object, event);
}

void ConsoleWidget::execute(const QString & command)
{
    QString cmd = command + "\n";
//    ui->pte_output->append(cmd);
    bash->write(cmd.toUtf8());
}

void ConsoleWidget::write_output()
{
    QByteArray bytes = bash->readAllStandardOutput();
    QString string = QString::fromUtf8(bytes.constData());


    current_console_line += string.count('\n');

    QString text = ui->pte_output->toPlainText();

    if(current_console_line > max_console_line)
    {
        ui->pte_output->clear();

        int diff = qAbs(max_console_line - current_console_line);
        qDebug() << "diff : " << diff;

        int last_index = 0;
        for (int i = 0; i < diff; ++i) {
            int found = 0;
            found = text.indexOf('\n', last_index+1);
            if(found==-1)
                break;
            else{
                last_index = found;
            }
        }

        qDebug() << "last_index: " << last_index;

        text.remove(0, last_index);
        string.prepend(text);
        current_console_line = string.count('\n');
    }

    ui->pte_output->appendPlainText(string);
    ui->pte_output->verticalScrollBar()->setValue(ui->pte_output->verticalScrollBar()->maximum());
}


