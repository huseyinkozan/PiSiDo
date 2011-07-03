#include "consolewidget.h"
#include "ui_consolewidget.h"

#include <QKeyEvent>
#include <QProcess>

ConsoleWidget::ConsoleWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConsoleWidget)
{
    ui->setupUi(this);

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
    ui->te_output->append(cmd);
    bash->write(cmd.toUtf8());
}

void ConsoleWidget::write_output()
{
    QByteArray bytes = bash->readAllStandardOutput();
    ui->te_output->append(QString::fromUtf8(bytes.constData()));
}
