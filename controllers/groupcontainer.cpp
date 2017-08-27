#include "groupcontainer.h"
#include "controllers/db.h"
#include "controllers/datecontrollers.h"
#include "controllers/taskcontainer.h"
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

void GroupContainer::removeGroup(int id)
{
    int group_index = findGroup(id, QString(typeid(GroupContainer).name()) + "::" + QString(__func__));

    for (int i = 0; i < task_container.taskCount(); i++) {
        if (task_container.getTask(i).group_id == id)
            task_container.removeTask(task_container.getTask(i).id);
    }

    db.deleteGroup(_group_vec[group_index]);
    _group_vec.erase(_group_vec.begin() + group_index);
}

size_t GroupContainer::groupCount()
{
    return _group_vec.size();
}
