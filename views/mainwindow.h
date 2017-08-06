#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "models/task.h"

class TaskDialog;
class QPushButton;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    void openTaskDlg();
    QPushButton* genDayBtn(QDate chosen_date);
    QPushButton* genEmptyDayBtn();
    void drawDayButtons();
    QWidget* genDayTask(const DayTask& daytask);
    void drawDayTasks();
    void redrawDate();
    void initConnects();

    Ui::MainWindow *ui;
    TaskDialog* task_dlg;

    QDate current_date;
    std::vector<QPushButton*> daybutton_vec;
    std::vector<QWidget*> daytask_widget_vec;
    int chosen_daytask_id;
    bool is_month_window;
};

#endif // MAINWINDOW_H
