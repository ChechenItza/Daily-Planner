#ifndef TaskWIDGET_H
#define TaskWIDGET_H

#include <QWidget>
#include "controllers/datecontrollers.h"

class TaskWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TaskWidget(QDate date, Task Task, QWidget *parent = nullptr);

signals:
    void redrawNeeded();

public slots:

private:
    QDate current_date;
    bool redrawIsNeeded;
    void editTaskTime(int Task_id, QTime start_time, QTime duration);
};

#endif // TaskWIDGET_H
