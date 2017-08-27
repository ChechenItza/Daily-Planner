#ifndef TASKCONTAINER_H
#define TASKCONTAINER_H

#include <vector>
#include "models/tasktemplate.h"
#include "models/group.h"

class TaskTemplateContainer
{
public:
    static TaskTemplateContainer& getInstance();

    void addTaskTemplate(TaskTemplate task);
    void addTaskTemplateFromDb(TaskTemplate task);
    TaskTemplate getTaskTemplate(int index);
    TaskTemplate findTaskTemplate(int id);
    void setName(int id, QString name);
    void setGroup(int id, int group_id);
    Group getGroup(int id);
    void setIconPath(int id, QString path);
    void removeTaskTemplate(int id);
    void linkTask(int id, QDate date, int Task_id);

    size_t taskCount();

private:
    TaskTemplateContainer() = default;

    std::vector<TaskTemplate> _task_vec;
    int findTaskTemplate(int id, QString method);
};
#define tasktemplate_container TaskTemplateContainer::getInstance()

#endif // TASKCONTAINER_H
