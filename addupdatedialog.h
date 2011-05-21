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

private:
    Ui::AddUpdateDialog *ui;
};

#endif // ADDUPDATEDIALOG_H
