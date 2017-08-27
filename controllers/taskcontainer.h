#ifndef TASKCONTAINER_H
#define TASKCONTAINER_H

#include <vector>
#include "models/task.h"
#include "models/group.h"

class TaskContainer
{
public:
    static TaskContainer& getInstance();

    void addTask(Task task);
    void addTaskFromDb(Task task);
    Task getTask(int index);
    Task findTask(int id);
    void setName(int id, QString name);
    void setGroup(int id, int group_id);
    Group getGroup(int id);
    void setIconPath(int id, QString path);
    void removeTask(int id);
    void linkDayTask(int id, QDate date, int daytask_id);

    size_t taskCount();

private:
    TaskContainer() = default;

    std::vector<Task> _task_vec;
    int findTask(int id, QString method);
};
#define task_container TaskContainer::getInstance()

#endif // TASKCONTAINER_H
