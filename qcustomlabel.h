#ifndef QCUSTOMLABEL_H
#define QCUSTOMLABEL_H
#include <QLabel>

class QCustomLabel : public QLabel
{
public:
    QCustomLabel() = default;
    QCustomLabel(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    QCustomLabel(QWidget* w);
};

#endif // QCUSTOMLABEL_H
