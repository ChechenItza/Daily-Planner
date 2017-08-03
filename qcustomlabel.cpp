#include "qcustomlabel.h"
#include "constants.h"

QCustomLabel::QCustomLabel(const QString &text, QWidget* parent, Qt::WindowFlags f) :
    QLabel(text, parent, f)
{
    setStyleSheet("color: " + Constants::black + ";");
}

QCustomLabel::QCustomLabel(QWidget *w) : QLabel(w) {}

