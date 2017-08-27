#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDate>
#include "models/tasktemplate.h"

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
    void drawDayButtons();
    void drawTasks();
    void redrawDate();
    void initConnects();

    Ui::MainWindow *ui;
    TaskDialog* task_dlg;

    QDate current_date;
    std::vector<QPushButton*> daybutton_vec;
    std::vector<QWidget*> task_widget_vec;
    int chosen_Task_id;
    bool is_month_window;
};

#endif // MAINWINDOW_H
