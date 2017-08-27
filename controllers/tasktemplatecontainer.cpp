#include "tasktemplatecontainer.h"
#include "controllers/db.h"
#include "controllers/datecontrollers.h"
#include "controllers/groupcontainer.h"
#include <QMessageBox>
#include <typeinfo>

namespace {
    void genError(int id, QString method)
    {
        QMessageBox error_msg(QMessageBox::Critical, method, "Given id does not exist");
        error_msg.exec();
        qDebug() << id;
        throw;
    }
}


TaskTemplateContainer& TaskTemplateContainer::getInstance()
{
    static TaskTemplateContainer tasktemplate_container_instance;
    return tasktemplate_container_instance;
}

int TaskTemplateContainer::findTaskTemplate(int id, QString method)
{
    for (int i = 0; i < _task_vec.size(); i++) {
        if (_task_vec[i].id == id)
            return i;
    }
    genError(id, method);
}

void TaskTemplateContainer::addTaskTemplate(TaskTemplate task)
{
    if (_task_vec.empty())
        task.id = 0;
    else
        task.id = _task_vec[_task_vec.size() - 1].id + 1;

    _task_vec.push_back(task);
    db.insertTaskTemplate(task);
}

void TaskTemplateContainer::addTaskTemplateFromDb(TaskTemplate task)
{
    _task_vec.push_back(task);
}

TaskTemplate TaskTemplateContainer::getTaskTemplate(int index)
{
    return _task_vec[index];
}

TaskTemplate TaskTemplateContainer::findTaskTemplate(int id)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    return _task_vec[task_index];
}

void TaskTemplateContainer::setName(int id, QString name)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].name = name;
    db.updateTaskTemplate(_task_vec[task_index]);
}

void TaskTemplateContainer::setGroup(int id, int group_id)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].group_id = group_id;
    db.updateTaskTemplate(_task_vec[task_index]);
}

Group TaskTemplateContainer::getGroup(int id)
{
    int group_id = _task_vec[findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__))].group_id;
    for (int i = 0; i < group_container.groupCount(); i++)
        if (group_container.getGroup(i).id == group_id)
            return group_container.getGroup(i);
    genError(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
}

void TaskTemplateContainer::setIconPath(int id, QString path)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].icon_path = path;
    db.updateTaskTemplate(_task_vec[task_index]);
}

void TaskTemplateContainer::removeTaskTemplate(int id)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    for (std::pair<QDate, int> Task_info : _task_vec[task_index].linked_Tasks_vec)
        TaskController::removeTask(Task_info.first, Task_info.second);

    db.deleteTaskTemplate(_task_vec[task_index]);
    _task_vec.erase(_task_vec.begin() + task_index);
}

void TaskTemplateContainer::linkTask(int id, QDate date, int Task_id)
{
    int task_index = findTaskTemplate(id, QString(typeid(TaskTemplateContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].linked_Tasks_vec.push_back(std::make_pair(date, Task_id));
}

size_t TaskTemplateContainer::taskCount()
{
    return _task_vec.size();
}
