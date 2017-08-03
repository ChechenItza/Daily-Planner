#ifndef ICONBUTTON_H
#define ICONBUTTON_H
#include <QPushButton>
#include <QIcon>
#include <QWidget>

class IconButton : public QPushButton
{
    Q_OBJECT
public:
    IconButton(QIcon icon, QWidget* parent = 0);
    IconButton(QWidget* parent = 0);
};

#endif // ICONBUTTON_H
