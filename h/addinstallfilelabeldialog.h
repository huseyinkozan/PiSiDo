#ifndef ADDINSTALLFILELABELDIALOG_H
#define ADDINSTALLFILELABELDIALOG_H

#include <QDialog>

namespace Ui {
    class AddInstallFileLabelDialog;
}

class AddInstallFileLabelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddInstallFileLabelDialog(QWidget * parent, QString path);
    ~AddInstallFileLabelDialog();

    QString get_file_type();
    bool get_permanent();

private slots:
    void buttonBox_accepted();

private:
    Ui::AddInstallFileLabelDialog *ui;
};

#endif // ADDINSTALLFILELABELDIALOG_H
