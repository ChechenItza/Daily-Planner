#ifndef TASK_H
#define TASK_H

#include <QString>
#include <vector>
#include <QDate>

struct TaskTemplate
{
    QString name;
    int group_id;
    QString icon_path;
    int id;
    std::vector<std::pair<QDate, int>> linked_Tasks_vec;
};

#endif // TASK_H
