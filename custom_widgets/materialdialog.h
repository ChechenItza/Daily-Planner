#ifndef MATERIALDIALOG_H
#define MATERIALDIALOG_H

#include <QDialog>
class QLayout;

namespace Ui {
class MaterialDialog;
}

class MaterialDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaterialDialog(QWidget *parent = 0);
    ~MaterialDialog();

    void insertLayout(QLayout* layout);
    void setHeading(QString heading);
    void setCancelBtnText(QString text);
    void setConfirmBtnText(QString text);

signals:
    void cancelBtnClicked();
    void confirmBtnClicked();

private:
    Ui::MaterialDialog *ui;
};

#endif // MATERIALDIALOG_H
