#ifndef Task_H
#define Task_H

#include <QTime>
#include <QString>
#include <vector>
#include <QString>
#include "models/break.h"

class Task
{
public:
    Task() = default;
    Task(int tasktemplate_id, QTime start_time, QTime duration, int is_done = 0);
    //From DB
    Task(int id, int tasktemplate_id, QString note, QTime start_time, QTime duration, int is_done);

    int id;
    int tasktemplate_id;
    QString note;
    QTime start_time;
    QTime duration;
    int is_done;
    std::vector<Break> break_vec;
};
#endif // Task_H
