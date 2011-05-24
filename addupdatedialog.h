#ifndef ADDUPDATEDIALOG_H
#define ADDUPDATEDIALOG_H

#include <QDialog>

namespace Ui {
    class AddUpdateDialog;
}

class AddUpdateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddUpdateDialog(QWidget *parent = 0);
    ~AddUpdateDialog();

    QString get_date();
    QString get_version();
    QString get_comment();
    QString get_packager_name();
    QString get_packager_email();

private:
    Ui::AddUpdateDialog *ui;
};

#endif // ADDUPDATEDIALOG_H
