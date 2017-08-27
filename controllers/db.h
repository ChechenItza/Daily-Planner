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

    void insertTaskTemplate(TaskTemplate task);
    void updateTaskTemplate(TaskTemplate task);
    void deleteTaskTemplate(TaskTemplate task);

    void insertTask(QDate date, Task task);
    void updateTask(QDate date, Task task);
    void deleteTask(QDate date, Task task);

    void insertBreak(QDate date, int task_id, Break brk);
    void deleteBreak(QDate date, int Task_id, Break brk);

    void updateDay(QDate date, Day day);

    void insertGroup(Group group);
    void updateGroup(Group group);
    void deleteGroup(Group group);

    void commit();

private:
    DbController();

    QSqlDatabase data_db;
};
#define db DbController::dbSingleton()

#endif // DB_H
