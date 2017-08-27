#include "task.h"

//DayTask
Task::Task(int task_id, QTime start_time, QTime duration, int is_done) :
    task_id{task_id},
    note{""},
    start_time{start_time},
    duration{duration},
    is_done{is_done} {}

//From DB
Task::Task(int id, int task_id, QString note, QTime start_time, QTime duration, int is_done) :
    id{id},
    task_id{task_id},
    note{note},
    start_time{start_time},
    duration{duration},
    is_done{is_done} {}
