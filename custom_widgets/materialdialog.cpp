#include "materialdialog.h"
#include "ui_materialdialog.h"
#include <QLayout>
#include <QStyle>
#include <QDesktopWidget>

MaterialDialog::MaterialDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaterialDialog)
{
    ui->setupUi(this);
    this->setModal(true);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFont(QFont("Roboto Medium", 10));
    this->setStyleSheet("color: rgba(0, 0, 0,  66%);");
//    //center the window
//    this->move(
//       parent->window()->frameGeometry().topLeft() +
//       parent->window()->rect().center() - rect().center()
//    );

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
    layout->setContentsMargins(24, 20, 24, 24);
    layout->setSpacing(8);

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
