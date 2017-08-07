#include "models/task.h"
#include "controllers/db.h"
#include "controllers/datecontrollers.h"
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

//GroupContainer {{{
GroupContainer& GroupContainer::getInstance()
{
    static GroupContainer group_container_instance;
    return group_container_instance;
}

int GroupContainer::findGroup(int id, QString method)
{
    for (int i = 0; i < groupCount(); i++) {
        if (_group_vec[i].id == id)
            return i;
    }
    genError(id, method);
}

void GroupContainer::addGroup(Group group)
{
    if (_group_vec.empty())
        group.id = 0;
    else
        group.id = _group_vec[groupCount() - 1].id + 1;

    _group_vec.push_back(group);
    db.insertGroup(group);
}

void GroupContainer::addGroupFromDb(Group group)
{
    _group_vec.push_back(group);
}

Group GroupContainer::getGroup(int index)
{
    return _group_vec[index];
}

void GroupContainer::setName(int id, QString name)
{
    int group_index = findGroup(id, QString(typeid(GroupContainer).name()) + "::" + QString(__func__));
    _group_vec[group_index].name = name;
    db.updateGroup(_group_vec[group_index]);
}

void GroupContainer::setColor(int id, QString color)
{
    int group_index = findGroup(id, QString(typeid(GroupContainer).name()) + "::" + QString(__func__));
    _group_vec[group_index].color = color;
    db.updateGroup(_group_vec[group_index]);
}

size_t GroupContainer::groupCount()
{
    return _group_vec.size();
}
// }}} GroupContainer

// TaskContainer {{{
TaskContainer& TaskContainer::getInstance()
{
    static TaskContainer task_container_instance;
    return task_container_instance;
}

int TaskContainer::findTask(int id, QString method)
{
    for (int i = 0; i < _task_vec.size(); i++) {
        if (_task_vec[i].id == id)
            return i;
    }
    genError(id, method);
}

void TaskContainer::addTask(Task task)
{
    if (_task_vec.empty())
        task.id = 0;
    else
        task.id = _task_vec[_task_vec.size() - 1].id + 1;

    _task_vec.push_back(task);
    db.insertTask(task);
}

void TaskContainer::addTaskFromDb(Task task)
{
    _task_vec.push_back(task);
}

Task TaskContainer::getTask(int index)
{
    return _task_vec[index];
}

Task TaskContainer::findTask(int id)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    return _task_vec[task_index];
}

void TaskContainer::setText(int id, QString text)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].text = text;
    db.updateTask(_task_vec[task_index]);
}

void TaskContainer::setGroup(int id, Group group)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].group_id = group.id;
    db.updateTask(_task_vec[task_index]);
}

Group TaskContainer::getGroup(int id)
{
    int group_id = _task_vec[findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__))].group_id;
    for (int i = 0; i < group_container.groupCount(); i++)
        if (group_container.getGroup(i).id == group_id)
            return group_container.getGroup(i);
        else
            genError(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
}

void TaskContainer::setIconPath(int id, QString path)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].icon_path = path;
    db.updateTask(_task_vec[task_index]);
}

void TaskContainer::removeTask(int id)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    for (std::pair<QDate, int> daytask_info : _task_vec[task_index].linked_daytasks_vec)
        DayTaskController::removeDayTask(daytask_info.first, daytask_info.second);

    db.deleteTask(_task_vec[task_index]);
    _task_vec.erase(_task_vec.begin() + task_index);
}

void TaskContainer::linkDayTask(int id, QDate date, int daytask_id)
{
    int task_index = findTask(id, QString(typeid(TaskContainer).name()) + "::" + QString(__func__));
    _task_vec[task_index].linked_daytasks_vec.push_back(std::make_pair(date, daytask_id));
}

size_t TaskContainer::taskCount()
{
    return _task_vec.size();
}
// }}} TaskContainer

//DayTask
DayTask::DayTask(int task_id, QTime start_time, QTime end_time) :
    task_id{task_id},
    note{""},
    start_time{start_time},
    end_time{end_time} {}

//From DB
DayTask::DayTask(int id, int task_id, QString note, QTime start_time, QTime end_time) :
    id{id},
    task_id{task_id},
    note{note},
    start_time{start_time},
    end_time{end_time} {}
