#include <QMessageBox>
#include <sys/stat.h>
#include <QDebug>
#include "controllers/db.h"
#include "models/tasktemplate.h"
#include "models/date.h"
#include "settings/constants.h"
#include "controllers/datecontrollers.h"
#include "controllers/groupcontainer.h"
#include "controllers/tasktemplatecontainer.h"

DbController::DbController()
{
//Prepare database
    std::string db_path = QDir::currentPath().toUtf8().constData();
    db_path += "/data.db";
    data_db = QSqlDatabase::addDatabase("QSQLITE");
    data_db.setDatabaseName(QString(db_path.c_str()));

//If database exists, call make(). If not, call init()
    struct stat buffer;
    if (stat(db_path.c_str(), &buffer) != 0)
        create();
    else
        init();

//Query all the changes to the database and exec them all at Database::commit() call
    data_db.transaction();
}

DbController::~DbController()
{
    commit();
    data_db.close();
}

void DbController::create()
{
    data_db.open();
    data_db.transaction();
    QSqlQuery init_query;

    init_query.prepare("PRAGMA foreign_keys = ON;");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE TABLE `Groups` ( "
                      "`Id`     INTEGER NOT NULL PRIMARY KEY UNIQUE, "
                      "`Name`   TEXT UNIQUE,"
                      "`Color`  TEXT)");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE TABLE `TaskTemplates` ( "
                      "`Id`     INTEGER NOT NULL PRIMARY KEY UNIQUE, "
                      "`Text`   TEXT,"
                      "`GroupId`INTEGER,"
                      "`Icon`   TEXT,"
                      "FOREIGN KEY(GroupId) REFERENCES Groups(Id))");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE TABLE `Tasks` ( "
                      "`Id`         INTEGER NOT NULL, "
                      "`Year`       INTEGER,"
                      "`Month`      INTEGER,"
                      "`Day`        INTEGER,"
                      "`TaskTemplateId`     INTEGER,"
                      "`Note`       BLOB,"
                      "`StartTime`  TEXT,"
                      "`Duration`   TEXT,"
                      "`IsDone`     INTEGER,"
                      "FOREIGN KEY(TaskTemplateId) REFERENCES Tasks(Id))");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE TABLE `DayBreaks` ( "
                      "`Id`         INTEGER NOT NULL,"
                      "`Year`       INTEGER,"
                      "`Month`      INTEGER,"
                      "`Day`        INTEGER,"
                      "`TaskId`  INTEGER,"
                      "`StartTime`  TEXT,"
                      "`EndTime`    TEXT,"
                      "FOREIGN KEY(TaskId) REFERENCES Tasks(Id))");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE TABLE `Days` ( "
                      "`Year`   INTEGER,"
                      "`Month`  INTEGER,"
                      "`Day`    INTEGER,"
                      "`Status` INTEGER,"
                      "`Note`   BLOB)");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    init_query.prepare("CREATE UNIQUE INDEX idx_unq ON Days(Year, Month, Day)");
    if (!init_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while creating the database. "
                              "Program will be closed.");
        qDebug() << init_query.lastError() << "\n" << init_query.lastQuery();
        error_msg.exec();
        throw std::runtime_error("Error in the database schema @Database::create()");
    }

    //Initial group (! this call is made from a static object to another static object)
    init_query.exec("INSERT INTO Groups (Id, Name, Color) VALUES (0, 'Miscellaneous', '" + constants::red + "')");
    group_container.addGroupFromDb(Group{"Miscellaneous", constants::red, 0});
    //Initial task  (! this call is made from a static object to another static object)
    init_query.exec("INSERT INTO TaskTemplates (Id, Text, GroupId, Icon) VALUES (0, 'Eat', 0, 'icons/soup.png')");
    tasktemplate_container.addTaskTemplateFromDb(TaskTemplate{"Eat", 0, "icons/soup.png", 0});

    data_db.commit();
}

void DbController::init()
{
    data_db.open();
    QSqlQuery init_query;
    init_query.exec("SELECT * FROM Groups");
    while (init_query.next()) {
        group_container.addGroupFromDb(Group{init_query.value(1).toString(),    //name
                                    init_query.value(2).toString(),             //color
                                    init_query.value(0).toInt()});              //id

    }

    init_query.exec("SELECT * FROM TaskTemplates");
    while (init_query.next()) {
        tasktemplate_container.addTaskTemplateFromDb(TaskTemplate{init_query.value(1).toString(),   //name
                                init_query.value(2).toInt(),                //group
                                init_query.value(3).toString(),             //icon
                                init_query.value(0).toInt()});              //id

    }

    init_query.exec("SELECT * FROM Tasks");
    while (init_query.next()) {
        TaskController::addTaskFromDb(
                    QDate(init_query.value(1).toInt(),              //year
                          init_query.value(2).toInt(),              //month
                          init_query.value(3).toInt()),             //day
                    Task(init_query.value(0).toInt(),            //id
                            init_query.value(4).toInt(),            //task_id
                            init_query.value(5).toString(),         //note
                            init_query.value(6).toTime(),           //start_time
                            init_query.value(7).toTime(),           //duration
                            init_query.value(8).toInt()));          //is_done
    }

    init_query.exec("SELECT * FROM Days");
    while (init_query.next()) {
        switch (init_query.value(3).toInt()) {
            case 0:
                DayController::setStatus(QDate(init_query.value(0).toInt(),
                                               init_query.value(1).toInt(),
                                               init_query.value(2).toInt()), constants::status_t::none, true);
                break;
            case 1:
                DayController::setStatus(QDate(init_query.value(0).toInt(),
                                               init_query.value(1).toInt(),
                                               init_query.value(2).toInt()), constants::status_t::bad, true);
                break;
            case 2:
                DayController::setStatus(QDate(init_query.value(0).toInt(),
                                               init_query.value(1).toInt(),
                                               init_query.value(2).toInt()), constants::status_t::ok, true);
                break;
            case 3:
                DayController::setStatus(QDate(init_query.value(0).toInt(),
                                               init_query.value(1).toInt(),
                                               init_query.value(2).toInt()), constants::status_t::good, true);
                break;
        }
        DayController::setNote(QDate(init_query.value(0).toInt(),
                                     init_query.value(1).toInt(),
                                     init_query.value(2).toInt()), init_query.value(4).toString(), true);
    }

    init_query.exec("SELECT * FROM DayBreaks");
    while (init_query.next()) {
        BreakController::addBreakFromDb(QDate(init_query.value(1).toInt(),      //year
                                              init_query.value(2).toInt(),      //month
                                              init_query.value(3).toInt()),     //day
                                        init_query.value(4).toInt(),            //Task_id
                                        Break{init_query.value(5).toTime(),     //start_time
                                              init_query.value(6).toTime(),     //end_time
                                              init_query.value(0).toInt()});    //id
    }
}

void DbController::insertTaskTemplate(TaskTemplate task)
{
    //Save task to the database
    QSqlQuery insert_query;
    insert_query.prepare("INSERT INTO TaskTemplates (Id, Text, GroupId, Icon) VALUES (:id, :task, :group_id, :icon)");
    insert_query.bindValue(":id", task.id);
    insert_query.bindValue(":task", task.name);
    insert_query.bindValue(":group_id", task.group_id);
    insert_query.bindValue(":icon", task.icon_path);
    if (!insert_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while adding created task to the database. "
                              "Program won't close, but created task won't be saved");
        qDebug() << insert_query.lastError() << "\n" << insert_query.lastQuery();
        error_msg.exec();
    }
}

void DbController::updateTaskTemplate(TaskTemplate task)
{
    //Update task in the database
    QSqlQuery update_query;
    update_query.prepare("UPDATE TaskTemplates SET Text = :title, GroupId = :group, Icon = :icon WHERE Id = :id");
    update_query.bindValue(":id", task.id);
    update_query.bindValue(":title", task.name);
    update_query.bindValue(":group", task.group_id);
    update_query.bindValue(":icon", task.icon_path);
    if (!update_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while adding created task to the database. "
                              "Program won't close, but created task won't be saved");
        error_msg.exec();
        qDebug() << update_query.lastError() << "\n" << update_query.lastQuery();
    }
}

void DbController::deleteTaskTemplate(TaskTemplate task)
{
    QSqlQuery delete_query;
    delete_query.prepare("DELETE FROM TaskTemplates WHERE Id = :id");
    delete_query.bindValue(":id", task.id);
    if (!delete_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while deleting task from the database. "
                              "Program won't close, but task won't be deleted");
        error_msg.exec();
        qDebug() << delete_query.lastError() << "\n" << delete_query.lastQuery();
    }
}

void DbController::insertTask(QDate date, Task Task)
{
    QSqlQuery insert_query;
    insert_query.prepare("INSERT INTO Tasks (Id, Year, Month, Day, TaskTemplateId, Note, StartTime, Duration, IsDone) VALUES ("
                         ":id, :y, :m, :d, :task_id, :note, :st, :du, :isd)");
    insert_query.bindValue(":id", QString::number(Task.id));
    insert_query.bindValue(":y", date.year());
    insert_query.bindValue(":m", date.month());
    insert_query.bindValue(":d", date.day());
    insert_query.bindValue(":task_id", Task.tasktemplate_id);
    insert_query.bindValue(":note", Task.note);
    insert_query.bindValue(":st", Task.start_time);
    insert_query.bindValue(":du", Task.duration);
    insert_query.bindValue(":isd", Task.is_done);
    if (!insert_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while adding created day task to the database. "
                              "Program won't close, but created day task won't be saved");
        error_msg.exec();
        qDebug() << insert_query.lastError() << "\n" << insert_query.lastQuery() << "\n" << insert_query.executedQuery();
    }
}

void DbController::updateTask(QDate date, Task Task)
{
    QSqlQuery update_query;
    update_query.prepare("UPDATE Tasks SET Note = :note, StartTime = :start_time, Duration = :duration, IsDone = :is_done WHERE "
                            "Year = :y AND "
                            "Month = :m AND "
                            "Day = :d AND "
                            "Id = :id");
    update_query.bindValue(":y", date.year());
    update_query.bindValue(":m", date.month());
    update_query.bindValue(":d", date.day());
    update_query.bindValue(":id", Task.id);
    update_query.bindValue(":note", Task.note);
    update_query.bindValue(":start_time", Task.start_time);
    update_query.bindValue(":duration", Task.duration);
    update_query.bindValue(":is_done", Task.is_done);
    if (!update_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while updating day task in the database. "
                              "Program won't close, but day task changes won't be saved");
        error_msg.exec();
        qDebug() << update_query.lastError() << "\n" << update_query.lastQuery();
    }
}

void DbController::deleteTask(QDate date, Task Task)
{
    QSqlQuery delete_query;
    delete_query.prepare("DELETE FROM Tasks WHERE Year = :y AND Month = :m AND Day = :d AND Id = :id");
    delete_query.bindValue(":y", date.year());
    delete_query.bindValue(":m", date.month());
    delete_query.bindValue(":d", date.day());
    delete_query.bindValue(":id", Task.id);
    if (!delete_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while deleting day task from the database. "
                              "Program won't close, but day task won't be deleted");
        error_msg.exec();
        qDebug() << delete_query.lastError() << "\n" << delete_query.lastQuery();
    }
}

void DbController::insertBreak(QDate date, int task_id, Break brk)
{
    QSqlQuery insert_query;
    insert_query.prepare("INSERT INTO DayBreaks (Year, Month, Day, TaskId, Id, StartTime, EndTime) VALUES ("
                             ":y, :m, :d, :task_id, :id, :st, :et)");
    //insert_query.bindValue(":id", brk.id);
    insert_query.bindValue(":y", date.year());
    insert_query.bindValue(":m", date.month());
    insert_query.bindValue(":d", date.day());
    insert_query.bindValue(":task_id", task_id);
    insert_query.bindValue(":id", brk.id);
    insert_query.bindValue(":st", brk.start_time);
    insert_query.bindValue(":et", brk.end_time);
    if (!insert_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while saving day breaks to the database. "
                                                                "Program won't close, but day breaks won't be saved");
        error_msg.exec();
        qDebug() << insert_query.lastError() << "\n" << insert_query.lastQuery();
    }
}

//void Database::updateBreak(QDate date, int task_id, Break brk)
//{
//    //Save day breaks
//    QSqlQuery insert_query;
//    insert_query.prepare("UPDATE DayBreaks SET StartTime = :st, EndTime = :et WHERE StartTime = :old_st("
//                             ":y, :m, :d, :task_id, :st, :et)");
//    //insert_query.bindValue(":id", brk.id);
//    insert_query.bindValue(":y", y);
//    insert_query.bindValue(":m", m);
//    insert_query.bindValue(":d", d);
//    insert_query.bindValue(":task_id", id);
//    insert_query.bindValue(":st", brk.start_time);
//    insert_query.bindValue(":et", brk.end_time);
//    if (!insert_query.exec()) {
//        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while saving day breaks to the database. "
//                                                                "Program won't close, but day breaks won't be saved");
//        error_msg.exec();
//        qDebug() << insert_query.lastError() << "\n" << insert_query.lastQuery();
//    }
//}

void DbController::deleteBreak(QDate date, int Task_id, Break brk)
{
    QSqlQuery delete_query;
    delete_query.prepare("DELETE FROM DayBreaks WHERE Year = :y AND Month = :m AND Day = :d AND TaskId = :Task_id AND Id = :id");
    delete_query.bindValue(":y", date.year());
    delete_query.bindValue(":m", date.month());
    delete_query.bindValue(":d", date.day());
    delete_query.bindValue(":Task_id", Task_id);
    delete_query.bindValue(":id", brk.id);

    if (!delete_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while deleting the break from the database. "
                              "Program won't close, but the break won't be deleted");
        error_msg.exec();
        qDebug() << delete_query.lastError() << "\n" << delete_query.lastQuery();
    }
}

void DbController::updateDay(QDate date, Day day)
{
    QSqlQuery update_query;
    update_query.prepare("INSERT INTO Days (Year, Month, Day, Status, Note) VALUES (:y, :m, :d, :status, :note)");
    update_query.bindValue(":y", date.year());
    update_query.bindValue(":m", date.month());
    update_query.bindValue(":d", date.day());
    update_query.bindValue(":status", day.status);
    update_query.bindValue(":note", day.note);
    if (!update_query.exec()) {
        update_query.prepare("UPDATE Days SET Status = :status, Note = :note WHERE "
                                "Year = :y AND "
                                "Month = :m AND "
                                "Day = :d");
        update_query.bindValue(":y", date.year());
        update_query.bindValue(":m", date.month());
        update_query.bindValue(":d", date.day());
        update_query.bindValue(":status", day.status);
        update_query.bindValue(":note", day.note);
        if (!update_query.exec()) {
            QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while updating day. "
                                    "Program won't close, but day changes won't be saved");
            error_msg.exec();
            qDebug() << update_query.lastError() << "\n" << update_query.lastQuery();
        }
    }
}

void DbController::insertGroup(Group group)
{
    QSqlQuery insert_query;
    insert_query.prepare("INSERT INTO Groups (Id, Name, Color) VALUES (:id, :name, :color)");
    insert_query.bindValue(":id", group.id);
    insert_query.bindValue(":name", group.name);
    insert_query.bindValue(":color", group.color);
    if (!insert_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while inserting group to the database. "
                              "Program won't close, but created group won't be saved");
        error_msg.exec();
        qDebug() << insert_query.lastError() << "\n" << insert_query.lastQuery();
    }
}

void DbController::updateGroup(Group group)
{
    QSqlQuery update_query;
    update_query.prepare("UPDATE Groups SET Name = :name, Color = :color WHERE Id = :id");
    update_query.bindValue(":id", group.id);
    update_query.bindValue(":name", group.name);
    update_query.bindValue(":color", group.color);
    if (!update_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while inserting group to the database. "
                              "Program won't close, but created group won't be saved");
        error_msg.exec();
        qDebug() << update_query.lastError() << "\n" << update_query.lastQuery();
    }
}

void DbController::deleteGroup(Group group)
{
    QSqlQuery delete_query;
    delete_query.prepare("DELETE FROM Groups WHERE Id = :id");
    delete_query.bindValue(":id", group.id);

    if (!delete_query.exec()) {
        QMessageBox error_msg(QMessageBox::Warning, "DB Error", "Error while deleting the group from the database. "
                              "Program won't close, but the group won't be deleted");
        error_msg.exec();
        qDebug() << delete_query.lastError() << "\n" << delete_query.lastQuery();
    }
}

void DbController::commit()
{
    data_db.commit();
}


DbController& DbController::dbSingleton()
{
    static DbController db_instance;
    return db_instance;
}
