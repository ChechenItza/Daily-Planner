#include <QMessageBox>
#include <typeinfo>
#include "controllers/tasktemplatecontainer.h"
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

    Task& findTask(QDate date, int id, QString method)
    {
        for (Task& task : date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec) {
            if (task.id == id)
                return task;
        }
        genError(id, method);
    }


    int findBreakIndex(QDate date, int Task_id, int id, QString method)
    {
        Task& task = findTask(date, Task_id, method);
        for (int i = 0; i < task.break_vec.size(); i++) {
            if (task.break_vec[i].id == id)
                return i;
        }
        genError(id, method);
    }

    int findTaskIndex(QDate date, int id, QString method)
    {
        for (int i = 0; i < date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.size(); i++) {
            if (date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec[i].id == id)
                return i;
        }
        genError(id, method);
    }
}

// TaskController {{{
void TaskController::addTask(QDate date, Task task)
{
    //set id
    if (date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.empty())
        task.id = 0;
    else
        task.id = date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec[TaskCount(date)-1].id + 1;

    //link this task to a template task
    tasktemplate_container.linkTask(task.tasktemplate_id, date, task.id);

    //add Task to model and database
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.push_back(task);
    db.insertTask(date, task);
}

void TaskController::addTaskFromDb(QDate date, Task task)
{
    //link this task to a template task
    tasktemplate_container.linkTask(task.tasktemplate_id, date, task.id);

    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.push_back(task);
}

Task TaskController::getTask(QDate date, int index)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec[index];
}

void TaskController::setNote(QDate date, int id, QString note)
{
    Task& task = findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__));
    task.note = note;
    db.updateTask(date, task);
}

bool TaskController::setStartTime(QDate date, int id, QTime start_time)
{
    Task& task = findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__));

    for (int i = 0; i < BreakController::breakCount(date, id); i++) {
        if (BreakController::getBreak(date, id, i).start_time < start_time)
            BreakController::removeBreak(date, id, BreakController::getBreak(date, id, i).id);
    }

    task.start_time = start_time;
    db.updateTask(date, task);

    return true;
}

bool TaskController::setDuration(QDate date, int id, QTime duration)
{
    Task& task = findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__));

    QTime end_time = task.start_time;
    int h_secs = duration.hour() * 60 * 60;
    int m_secs = duration.minute() * 60;
    end_time.addSecs(h_secs + m_secs);

    for (int i = 0; i < BreakController::breakCount(date, id); i++) {
        if (BreakController::getBreak(date, id, i).end_time > end_time)
            BreakController::removeBreak(date, id, BreakController::getBreak(date, id, i).id);
    }

    task.duration = duration;
    db.updateTask(date, task);

    return true;
}

void TaskController::setStatus(QDate date, int id, int is_done)
{
    Task& task = findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__));

    if (is_done < 3) {
        task.is_done = is_done;
        db.updateTask(date, task);
    }
}

void TaskController::removeTask(QDate date, int id)
{
    db.deleteTask(date, findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__)));
    date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.erase(
                date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.begin()
            + findTaskIndex(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__)));
}

QTime TaskController::getEndTime(QDate date, int id)
{
    Task task = findTask(date, id, QString(typeid(TaskController).name()) + "::" + QString(__func__));

    int h_secs = task.duration.hour() * 60 * 60;
    int m_secs = task.duration.minute() * 60;

    return task.start_time.addSecs(h_secs + m_secs);
}

size_t TaskController::TaskCount(QDate date)
{
    return date_singleton[date.year()-constants::START_YEAR][date.month()-1][date.day()-1].task_vec.size();
}
// }}} TaskContoller

// BreakController {{{
bool BreakController::addBreak(QDate date, int Task_id, Break brk)
{
    if (brk.start_time >= brk.end_time)
        return false;

    for (auto brk_from_model : findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec) {
        if ((brk.start_time == brk_from_model.start_time || brk.end_time == brk_from_model.end_time) ||
                (brk.start_time < brk_from_model.start_time && brk.end_time > brk_from_model.start_time) ||
                (brk.start_time < brk_from_model.end_time && brk.end_time > brk_from_model.end_time) ||
                (brk.start_time > brk_from_model.start_time && brk.end_time < brk_from_model.end_time))
            return false;
    }

    findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.push_back(brk);
    db.insertBreak(date, Task_id, brk);

    return true;
}

void BreakController::addBreakFromDb(QDate date, int Task_id, Break brk)
{
    findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.push_back(brk);
}

Break BreakController::getBreak(QDate date, int Task_id, int index)
{
    return findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec[index];
}

//void BreakController::setStartTime(QDate date, int Task_id, QTime old_start_time, QTime new_start_time)
//{
//    Break& brk = findBreakByStartTime(date, Task_id, old_start_time, "BreakController::setStartTime");
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

//void BreakController::setEndTime(QDate date, int Task_id, QTime old_end_time, QTime new_end_time)
//{

//}

void BreakController::removeBreak(QDate date, int Task_id, int id)
{
    Task& task = findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__));
    int break_index = findBreakIndex(date, Task_id, id, QString(typeid(BreakController).name()) + "::" + QString(__func__));

    db.deleteBreak(date, Task_id, task.break_vec[break_index]);
    task.break_vec.erase(task.break_vec.begin() + break_index);
}

size_t BreakController::breakCount(QDate date, int Task_id)
{
    return findTask(date, Task_id, QString(typeid(BreakController).name()) + "::" + QString(__func__)).break_vec.size();
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
