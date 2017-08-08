#include "materialdialog.h"
#include "ui_materialdialog.h"
#include <QLayout>

MaterialDialog::MaterialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaterialDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(ui->confirmBtn, &QPushButton::clicked, [this] {
        emit confirmBtnClicked();
    });
    connect(ui->cancelBtn, &QPushButton::clicked, this, &QWidget::close);
}

MaterialDialog::~MaterialDialog()
{
    delete ui;
}

void MaterialDialog::insertLayout(QLayout *layout)
{
    ui->verticalLayout->insertLayout(1, layout);
}

void MaterialDialog::setHeading(QString heading)
{
    ui->headingLbl->setText(heading);
}

void MaterialDialog::setCancelBtnText(QString text)
{
    ui->cancelBtn->setText(text);
}

void MaterialDialog::setConfirmBtnText(QString text)
{
    ui->confirmBtn->setText(text);
}
