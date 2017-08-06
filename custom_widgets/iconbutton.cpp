#include "custom_widgets/iconbutton.h"

IconButton::IconButton(QIcon icon, QWidget* parent) :
    QPushButton(icon, "", parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    this->setFlat(true);
}

IconButton::IconButton(QWidget* parent) :
    QPushButton("", parent)
{
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    this->setCursor(QCursor(Qt::CursorShape::PointingHandCursor));
    this->setFlat(true);
}

