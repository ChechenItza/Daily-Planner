#include "emptydaywidget.h"

EmptyDayWidget::EmptyDayWidget(QWidget* parent) :
    QPushButton(parent)
{
    this->setFlat(true);
    this->setMinimumSize(QSize(50, 50));
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    this->setStyleSheet("border: 1px solid rgb(189, 189, 189);"
                             "border-top: 0px;"
                             "border-left: 0px;");
}
