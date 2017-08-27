#include <QMessageBox>
#include <typeinfo>
#include "controllers/taskcontainer.h"
#include "controllers/datecontrollers.h"
#include "settings/constants.h"
#include "db.h"

namespace {
    void genError(int id, QString method)
    {
        QMessageBox error_msg(QMessageBox::Critical, method, "Given id does not exist");
        qDebug() << id;
        error_msg.exec();
        throw;
    }

    DayTask& findDayTask(QDate date, int id, QString method)
    {
        for (DayTask& daytask : date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec) {
            if (daytask.id == id)
                return daytask;
        }
        genError(id, method);
    }


    int findBreakIndex(QDate date, int daytask_id, int id, QString method)
    {
        DayTask& daytask = findDayTask(date, daytask_id, method);
        for (int i = 0; i < daytask.break_vec.size(); i++) {
            if (daytask.break_vec[i].id == id)
                return i;
        }
        genError(id, method);
    }

    int findDayTaskIndex(QDate date, int id, QString method)
    {
        for (int i = 0; i < date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.size(); i++) {
            if (date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec[i].id == id)
                return i;
        }
        genError(id, method);
    }
}

// DayTaskController {{{
void DayTaskController::addDayTask(QDate date, DayTask daytask)
{
    //set id
    if (date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.empty())
        daytask.id = 0;
    else
        daytask.id = date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec[dayTaskCount(date)-1].id + 1;

    //link this daytask to a template task
    task_container.linkDayTask(daytask.task_id, date, daytask.id);

    //add daytask to model and database
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.push_back(daytask);
    db.insertDayTask(date, daytask);
}

void DayTaskController::addDayTaskFromDb(QDate date, DayTask daytask)
{
    //link this daytask to a template task
    task_container.linkDayTask(daytask.task_id, date, daytask.id);

    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.push_back(daytask);
}

DayTask DayTaskController::getDayTask(QDate date, int index)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec[index];
}

void DayTaskController::setNote(QDate date, int id, QString note)
{
    DayTask& daytask = findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__));
    daytask.note = note;
    db.updateDayTask(date, daytask);
}

bool DayTaskController::setStartTime(QDate date, int id, QTime start_time)
{
    DayTask& daytask = findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__));

    for (int i = 0; i < BreakController::breakCount(date, id); i++) {
        if (BreakController::getBreak(date, id, i).start_time < start_time)
            BreakController::removeBreak(date, id, BreakController::getBreak(date, id, i).id);
    }

    daytask.start_time = start_time;
    db.updateDayTask(date, daytask);

    return true;
}

bool DayTaskController::setDuration(QDate date, int id, QTime duration)
{
    DayTask& daytask = findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__));

    QTime end_time = daytask.start_time;
    int h_secs = duration.hour() * 60 * 60;
    int m_secs = duration.minute() * 60;
    end_time.addSecs(h_secs + m_secs);

    for (int i = 0; i < BreakController::breakCount(date, id); i++) {
        if (BreakController::getBreak(date, id, i).end_time > end_time)
            BreakController::removeBreak(date, id, BreakController::getBreak(date, id, i).id);
    }

    daytask.duration = duration;
    db.updateDayTask(date, daytask);

    return true;
}

void DayTaskController::setStatus(QDate date, int id, int is_done)
{
    DayTask& daytask = findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__));

    if (is_done < 3) {
        daytask.is_done = is_done;
        db.updateDayTask(date, daytask);
    }
}

void DayTaskController::removeDayTask(QDate date, int id)
{
    db.deleteDayTask(date, findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__)));
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.erase(
                date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.begin()
            + findDayTaskIndex(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__)));
}

QTime DayTaskController::getEndTime(QDate date, int id)
{
    DayTask daytask = findDayTask(date, id, QString(typeid(DayTaskController).name()) + "::" + QString(__func__));

    int h_secs = daytask.duration.hour() * 60 * 60;
    int m_secs = daytask.duration.minute() * 60;

    return daytask.start_time.addSecs(h_secs + m_secs);
}

size_t DayTaskController::dayTaskCount(QDate date)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].daytask_vec.size();
}
// }}} DayTaskContoller

// BreakController {{{
bool BreakController::addBreak(QDate date, int daytask_id, Break brk)
{
    if (brk.start_time >= brk.end_time)
        return false;

    for (auto brk_from_model : findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec) {
        if ((brk.start_time == brk_from_model.start_time || brk.end_time == brk_from_model.end_time) ||
                (brk.start_time < brk_from_model.start_time && brk.end_time > brk_from_model.start_time) ||
                (brk.start_time < brk_from_model.end_time && brk.end_time > brk_from_model.end_time) ||
                (brk.start_time > brk_from_model.start_time && brk.end_time < brk_from_model.end_time))
            return false;
    }

    findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.push_back(brk);
    db.insertBreak(date, daytask_id, brk);

    return true;
}

void BreakController::addBreakFromDb(QDate date, int daytask_id, Break brk)
{
    findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.push_back(brk);
}

Break BreakController::getBreak(QDate date, int daytask_id, int index)
{
    return findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec[index];
}

//void BreakController::setStartTime(QDate date, int daytask_id, QTime old_start_time, QTime new_start_time)
//{
//    Break& brk = findBreakByStartTime(date, daytask_id, old_start_time, "BreakController::setStartTime");
//    if (new_start_time >= brk.end_time)
//        return false;

//    for (auto brk_from_model : break_vec) {
//        if ((new_start_time == brk_from_model.start_time || brk.end_time == brk_from_model.end_time) ||
//                (new_start_time < brk_from_model.start_time && brk.end_time > brk_from_model.start_time) ||
//                (new_start_time < brk_from_model.end_time && brk.end_time > brk_from_model.end_time) ||
//                (new_start_time > brk_from_model.start_time && brk.end_time < brk_from_model.end_time))
//            return false;
//    }

//    brk.start_time = new_start_time;
//    db.up
//}

//void BreakController::setEndTime(QDate date, int daytask_id, QTime old_end_time, QTime new_end_time)
//{

//}

void BreakController::removeBreak(QDate date, int daytask_id, int id)
{
    DayTask& daytask = findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__));
    int break_index = findBreakIndex(date, daytask_id, id, QString(typeid(BreakController).name()) + "::" + QString(__func__));

    db.deleteBreak(date, daytask_id, daytask.break_vec[break_index]);
    daytask.break_vec.erase(daytask.break_vec.begin() + break_index);
}

size_t BreakController::breakCount(QDate date, int daytask_id)
{
    return findDayTask(date, daytask_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.size();
}
// }}} BreakController

// DayController {{{
QString DayController::getNote(QDate date)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].note;
}

void DayController::setNote(QDate date, QString note, bool from_db)
{
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].note = note;

    if (!from_db)
        db.updateDay(date, date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1]);
}

constants::status_t DayController::getStatus(QDate date)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].status;
}

void DayController::setStatus(QDate date, constants::status_t status, bool from_db)
{
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].status = status;

    if (!from_db)
        db.updateDay(date, date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1]);
}
// }}} DayController
