#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#include <QWidget>

namespace Ui {
    class TranslationWidget;
}

class TranslationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationWidget(
                            QWidget *parent,
                            const QString & language);
    ~TranslationWidget();

    QString get_summary();
    QString get_description();
    QString get_language();

signals:
    void delete_me(ArchiveWidget *);

private slots:
    void on_tb_delete_clicked();

private:
    Ui::TranslationWidget *ui;
};

#endif // TRANSLATIONWIDGET_H
