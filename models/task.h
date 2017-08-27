#ifndef TASK_H
#define TASK_H

#include <QString>
#include <vector>
#include <QDate>

struct Task
{
    QString name;
    int group_id;
    QString icon_path;
    int id;
    std::vector<std::pair<QDate, int>> linked_daytasks_vec;
};

#endif // TASK_H
