#include "custom_widgets/myiconbutton.h"

MyIconButton::MyIconButton(QIcon icon, QWidget* parent) :
    QPushButton(icon, "", parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    this->setFlat(true);
}

MyIconButton::MyIconButton(QWidget* parent) :
    QPushButton("", parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    this->setFlat(true);
}

