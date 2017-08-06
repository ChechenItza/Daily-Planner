#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QTime>
#include <vector>
#include <QIcon>

struct Group
{
    QString name;
    QString color;
    int id;
};

class GroupContainer
{
public:
    static GroupContainer& getInstance();

    void addGroup(Group group);
    void addGroupFromDb(Group group);
    Group getGroup(int index);
    void setName(int id, QString name);
    void setColor(int id, QString color);

    size_t groupCount();

private:
    GroupContainer() = default;

    std::vector<Group> _group_vec;
    int findGroup(int id, QString method);
};
#define group_container GroupContainer::getInstance()

struct Task
{
    QString text;
    int group_id;
    QString icon_path;
    int id;

//signals:
//    void Deleted();
};

class TaskContainer
{
public:
    static TaskContainer& getInstance();

    void addTask(Task task);
    void addTaskFromDb(Task task);
    Task getTask(int index);
    Task findTask(int id);
    void setText(int id, QString text);
    void setGroup(int id, Group group);
    Group getGroup(int id);
    void setIconPath(int id, QString path);
    void removeTask(int id);

    size_t taskCount();

private:
    TaskContainer() = default;

    std::vector<Task> _task_vec;
    int findTask(int id, QString method);
};
#define task_container TaskContainer::getInstance()

struct Break
{
    QTime start_time;
    QTime end_time;
    int id;
};

class DayTask
{
public:
    DayTask() = default;
    DayTask(int task_id, QTime start_time, QTime end_time);
    //From DB
    DayTask(int id, int task_id, QString note, QTime start_time, QTime end_time);

    int id;
    int task_id;
    QString note;
    QTime start_time;
    QTime end_time;
    std::vector<Break> break_vec;
};

#endif // TASK_H
