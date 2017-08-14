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
    void removeGroup(int id);

    size_t groupCount();

private:
    GroupContainer() = default;

    std::vector<Group> _group_vec;
    int findGroup(int id, QString method);
};
#define group_container GroupContainer::getInstance()

struct Task
{
    QString name;
    int group_id;
    QString icon_path;
    int id;
    std::vector<std::pair<QDate, int>> linked_daytasks_vec;
};

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
    DayTask(int task_id, QTime start_time, QTime duration, int is_done = 0);
    //From DB
    DayTask(int id, int task_id, QString note, QTime start_time, QTime duration, int is_done);

    int id;
    int task_id;
    QString note;
    QTime start_time;
    QTime duration;
    int is_done;
    std::vector<Break> break_vec;
};

#endif // TASK_H
