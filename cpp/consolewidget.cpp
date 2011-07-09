#include "consolewidget.h"
#include "ui_consolewidget.h"

#include <QKeyEvent>
#include <QProcess>
#include <QSettings>
#include <QDebug>
#include <QScrollBar>
#include <QTimer>

#define BASH_RESTART_TIME 1000

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
    connect(bash, SIGNAL(finished(int)), SLOT(handle_bash_exit(int)));
    start_bash();
}

ConsoleWidget::~ConsoleWidget()
{
    bash->close();
    delete ui;
}

bool ConsoleWidget::eventFilter(QObject * object, QEvent * event)
{
    if(object == ui->le_input){
        if(event->type() == QEvent::KeyPress){
            QKeyEvent * key_event = static_cast<QKeyEvent *>(event);
            if((key_event->key() == Qt::Key_Return) || (key_event->key() == Qt::Key_Enter)){
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

void ConsoleWidget::start_bash()
{
    bash->start("bash", QStringList() << "-i");
}

void ConsoleWidget::handle_bash_exit(int exitCode)
{
    if(exitCode)
        qDebug() << "Bash exit code : " << exitCode;
    ui->pte_output->clear();
    QTimer::singleShot(BASH_RESTART_TIME, this, SLOT(start_bash()));
}

void ConsoleWidget::execute(const QString & command)
{
    QString cmd = command + "\n";
    bash->write(cmd.toUtf8());
}

void ConsoleWidget::write_output()
{
    QByteArray bytes = bash->readAllStandardOutput();
    QString string = QString::fromUtf8(bytes.constData());
    current_console_line += string.count('\n');
    QString text = ui->pte_output->toPlainText();
    if(current_console_line > max_console_line){
        ui->pte_output->clear();
        int diff = qAbs(max_console_line - current_console_line);
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
        text.remove(0, last_index);
        string.prepend(text);
        current_console_line = string.count('\n');
    }
    ui->pte_output->appendPlainText(string);
    ui->pte_output->verticalScrollBar()->setValue(ui->pte_output->verticalScrollBar()->maximum());
}

