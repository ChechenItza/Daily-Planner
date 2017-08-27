#ifndef TASKCONTAINER_H
#define TASKCONTAINER_H

#include <vector>
#include "models/tasktemplate.h"
#include "models/group.h"

class TaskContainer
{
public:
    static TaskContainer& getInstance();

    void addTask(TaskTemplate task);
    void addTaskFromDb(TaskTemplate task);
    TaskTemplate getTask(int index);
    TaskTemplate findTask(int id);
    void setName(int id, QString name);
    void setGroup(int id, int group_id);
    Group getGroup(int id);
    void setIconPath(int id, QString path);
    void removeTask(int id);
    void linkDayTask(int id, QDate date, int daytask_id);

    size_t taskCount();

private:
    TaskContainer() = default;

    std::vector<TaskTemplate> _task_vec;
    int findTask(int id, QString method);
};
#define task_container TaskContainer::getInstance()

#endif // TASKCONTAINER_H
