#ifndef DAYTASK_H
#define DAYTASK_H

#include <QTime>
#include <QString>
#include <vector>
#include <QString>
#include "models/break.h"

class Task
{
public:
    Task() = default;
    Task(int task_id, QTime start_time, QTime duration, int is_done = 0);
    //From DB
    Task(int id, int task_id, QString note, QTime start_time, QTime duration, int is_done);

    int id;
    int task_id;
    QString note;
    QTime start_time;
    QTime duration;
    int is_done;
    std::vector<Break> break_vec;
};
#endif // DAYTASK_H
