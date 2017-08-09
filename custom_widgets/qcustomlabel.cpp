#include "custom_widgets/qcustomlabel.h"
#include "settings/constants.h"

QCustomLabel::QCustomLabel(const QString &text, QWidget* parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
    setStyleSheet("color: " + constants::black + ";");
}

QCustomLabel::QCustomLabel(QWidget *w) : QLabel(w) {}

