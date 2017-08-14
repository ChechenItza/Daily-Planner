#ifndef DATECONTROLLER_H
#define DATECONTROLLER_H

#include "models/date.h"
#include "models/task.h"
class QDate;

class DayTaskController
{
public:
    static void addDayTask(QDate date, DayTask daytask);
    static void addDayTaskFromDb(QDate date, DayTask daytask);
    static DayTask getDayTask(QDate date, int index);
    static void setNote(QDate date, int id, QString note);
    static bool setStartTime(QDate date, int id, QTime start_time);
    static bool setDuration(QDate date, int id, QTime duration);
    static void setStatus(QDate date, int id, int is_done);
    static void removeDayTask(QDate date, int id);
    static QTime getEndTime(QDate date, int id);

    static size_t dayTaskCount(QDate date);
};

class BreakController
{
public:
    static bool addBreak(QDate date, int daytask_id, Break brk);
    static void addBreakFromDb(QDate date, int daytask_id, Break brk);
    static Break getBreak(QDate date, int daytask_id, int index);
//    static void setStartTime(QDate date, int daytask_id, QTime old_start_time, QTime new_start_time);
//    static void setEndTime(QDate date, int daytask_id, QTime old_end_time, QTime new_end_time);
    static void removeBreak(QDate date, int daytask_id, int id);

    static size_t breakCount(QDate date, int daytask_id);
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
