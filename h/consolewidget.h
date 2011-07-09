#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>

namespace Ui {
    class ConsoleWidget;
}

class QProcess;

class ConsoleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ConsoleWidget(QWidget *parent = 0);
    ~ConsoleWidget();
    virtual bool eventFilter(QObject *, QEvent *);

    void execute(const QString &command);

private slots:
    void write_output();
    void start_bash();
    void handle_bash_exit(int exitCode);

private:
    Ui::ConsoleWidget *ui;
    QProcess * bash;
    int current_console_line;
    int max_console_line;
};

#endif // CONSOLEWIDGET_H
