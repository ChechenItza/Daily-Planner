#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
struct TaskTemplate;
class Task;
struct Break;
class Day;
struct Group;


class DbController {
public:
    static DbController& dbSingleton();
    ~DbController();

    void create();
    void init();

    void insertTask(TaskTemplate task);
    void updateTask(TaskTemplate task);
    void deleteTask(TaskTemplate task);

    void insertDayTask(QDate date, Task daytask);
    void updateDayTask(QDate date, Task daytask);
    void deleteDayTask(QDate date, Task daytask);

    void insertBreak(QDate date, int task_id, Break brk);
    void deleteBreak(QDate date, int daytask_id, Break brk);

    void updateDay(QDate date, Day day);

    void insertGroup(Group);
    void updateGroup(Group);
    void deleteGroup(Group group);

    void commit();

private:
    DbController();

    QSqlDatabase data_db;
};
#define db DbController::dbSingleton()

#endif // DB_H
