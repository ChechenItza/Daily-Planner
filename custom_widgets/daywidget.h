#ifndef DAYWIDGET_H
#define DAYWIDGET_H

#include <QDate>
#include <QPushButton>

class DayWidget : public QPushButton
{
Q_OBJECT
public:
    explicit DayWidget(QDate date, QWidget *parent = nullptr);
    QDate getDate();

signals:
    void noteClicked();
    void redrawIsNeeded();

private:
    QDate chosen_date;
};

#endif // DAYWIDGET_H
