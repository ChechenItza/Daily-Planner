#ifndef DATECONTROLLER_H
#define DATECONTROLLER_H

#include "models/date.h"
#include "models/tasktemplate.h"
class QDate;

class TaskController
{
public:
    static void addTask(QDate date, Task task);
    static void addTaskFromDb(QDate date, Task task);
    static Task getTask(QDate date, int index);
    static void setNote(QDate date, int id, QString note);
    static bool setStartTime(QDate date, int id, QTime start_time);
    static bool setDuration(QDate date, int id, QTime duration);
    static void setStatus(QDate date, int id, int is_done);
    static void removeTask(QDate date, int id);
    static QTime getEndTime(QDate date, int id);

    static size_t TaskCount(QDate date);
};

class BreakController
{
public:
    static bool addBreak(QDate date, int Task_id, Break brk);
    static void addBreakFromDb(QDate date, int Task_id, Break brk);
    static Break getBreak(QDate date, int Task_id, int index);
//    static void setStartTime(QDate date, int Task_id, QTime old_start_time, QTime new_start_time);
//    static void setEndTime(QDate date, int Task_id, QTime old_end_time, QTime new_end_time);
    static void removeBreak(QDate date, int Task_id, int id);

    static size_t breakCount(QDate date, int Task_id);
};

class DayController
{
public:
    static QString getNote(QDate date);
    static void setNote(QDate date, QString note, bool from_db = false);
    static constants::status_t getStatus(QDate date);
    static void setStatus(QDate date, constants::status_t status, bool from_db = false);
};

#endif // DATECONTROLLER_H
