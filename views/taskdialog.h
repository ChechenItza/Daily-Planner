#ifndef TASKADDDIALOG_H
#define TASKADDDIALOG_H

#include <QDialog>
#include "models/group.h"
#include "models/tasktemplate.h"
class IconManager;
class QTimeEdit;

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
    void changed();

private:
    Ui::TaskAddDialog *ui;
    IconManager* icon_mng;
    QDate current_date;
    TaskTemplate current_task;

    void initConnects();
    void drawTasks();
    QWidget* genTask(TaskTemplate task);

    void addDayTask(int task_id, QTime start_time, QTime duration);
    void addTask(QString icon_path, QString name, int group_id);
    void editTask(int task_id, QString icon_path, QString name, int group_id);
    void addGroup(QString name, QString color);
    void drawGroups();
    QWidget *genGroup(Group group);
};

#endif // TASKADDDIALOG_H
