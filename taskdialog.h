#ifndef TASKADDDIALOG_H
#define TASKADDDIALOG_H

#include <QDialog>
#include "task.h"
class IconManager;

namespace Ui {
class TaskAddDialog;
}

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QDate current_date, QWidget *parent = 0);
    ~TaskDialog();

signals:
    void dayTaskAdded();

private:
    Ui::TaskAddDialog *ui;
    IconManager* icon_mng;
    QDate current_date;
    Task current_task;

    void initConnects();
    void drawTasks();
    QWidget* genTask(Task task);

    void addDayTask();
    void addTask();
};

#endif // TASKADDDIALOG_H
