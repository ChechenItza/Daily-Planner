#ifndef DAYTASKWIDGET_H
#define DAYTASKWIDGET_H

#include <QWidget>
#include "controllers/datecontrollers.h"

class DayTaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DayTaskWidget(QDate date, Task daytask, QWidget *parent = nullptr);

signals:
    void redrawNeeded();

public slots:

private:
    QDate current_date;
    void editDayTaskTime(int daytask_id, QTime start_time, QTime duration);
};

#endif // DAYTASKWIDGET_H
