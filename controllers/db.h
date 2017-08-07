#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
struct Task;
class DayTask;
struct Break;
class Day;
struct Group;


class DbController {
public:
    static DbController& dbSingleton();
    ~DbController();

    void create();
    void init();

    void insertTask(Task task);
    void updateTask(Task task);
    void deleteTask(Task task);

    void insertDayTask(QDate date, DayTask daytask);
    void updateDayTask(QDate date, DayTask daytask);
    void deleteDayTask(QDate date, DayTask daytask);

    void insertBreak(QDate date, int task_id, Break brk);
    void deleteBreak(QDate date, int daytask_id, Break brk);

    void updateDay(QDate date, Day day);

    void insertGroup(Group);
    void updateGroup(Group);

    void commit();

private:
    DbController();

    QSqlDatabase data_db;
};
#define db DbController::dbSingleton()

#endif // DB_H
