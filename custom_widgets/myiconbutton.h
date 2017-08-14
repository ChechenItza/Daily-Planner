#ifndef MYICONBUTTON_H
#define MYICONBUTTON_H
#include <QPushButton>
#include <QIcon>
#include <QWidget>

class MyIconButton : public QPushButton
{
    Q_OBJECT
public:
    MyIconButton(QIcon icon, QWidget* parent = 0);
    MyIconButton(QWidget* parent = 0);
};

#endif // MYICONBUTTON_H
