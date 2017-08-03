#include "taskdialog.h"
#include "ui_taskdialog.h"
#include "task.h"
#include "iconmanager.h"
#include "qcustomlabel.h"
#include "iconbutton.h"
#include "datecontrollers.h"
#include <QMessageBox>
#include <QScrollArea>

TaskDialog::TaskDialog(QDate current_date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskAddDialog),
    icon_mng(new IconManager),
    current_date{current_date}
{
    ui->setupUi(this);

    //fill %taskIconBox with icons
    QStringList icon_list = icon_mng->getIconList();
    for (int i = 0; i < icon_list.count(); i++) {
        ui->taskIconBox->addItem(QIcon(icon_list[i]), icon_list[i]);
    }

    //fill %taskGroupBox with groups
    for (int i = 0; i < group_container.groupCount(); i++) {
        ui->taskGroupBox->addItem(group_container.getGroup(i).name);
    }

    initConnects();
    drawTasks();
}

void TaskDialog::drawTasks()
{
    //remove all pages and tabs
    while (ui->tabWidget->count() != 0) {
        QWidget* page = ui->tabWidget->widget(0);
        delete page;
        ui->tabWidget->removeTab(0);
    }

    //page
    QScrollArea* tab_widget_scroll;
    QWidget* tab_widget;
    QVBoxLayout* tab_layout;

    for (int i = 0; i < group_container.groupCount(); i++) {
        //create page
        tab_widget_scroll = new QScrollArea();
        tab_widget_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        tab_widget_scroll->setWidgetResizable(true);
        tab_widget_scroll->setFrameShape(QFrame::Shape::NoFrame);
        tab_widget_scroll->setStyleSheet("QScrollArea { "
                                         "border-bottom: 1px solid #d2d2d2; "
                                         "border-top: 0px;"
                                         "border-right: 0px;"
                                         "border-left: 0px; }");

        tab_widget = new QWidget();
        tab_widget->setStyleSheet("background-color: #fafafa;");
        tab_widget_scroll->setWidget(tab_widget);

        tab_layout = new QVBoxLayout();
        tab_layout->setSpacing(0);
        tab_layout->setContentsMargins(0, 0, 0, 0);
        tab_widget->setLayout(tab_layout);

        //add task widgets to it
        for (int j = 0; j < task_container.taskCount(); j++) {
            if (task_container.getTask(j).group_id == group_container.getGroup(i).id)
                tab_layout->addWidget(genTask(task_container.getTask(j)));
        }
        QSpacerItem* task_spacer = new QSpacerItem(40, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        tab_layout->addSpacerItem(task_spacer);

        //add page to ui
        ui->tabWidget->addTab(tab_widget_scroll, group_container.getGroup(i).name);
    }
}

QWidget* TaskDialog::genTask(Task task)
{
    //container
    QWidget* task_widget = new QWidget();
    task_widget->setStyleSheet(".QWidget { border-bottom: 1px solid #d2d2d2; }");
    //container layout
    QHBoxLayout* task_layout = new QHBoxLayout(task_widget);
    task_layout->setSpacing(6);

    //icon
    QLabel* task_icon = new QLabel();
    QPixmap task_pixmap(task.icon_path);
    task_icon->setPixmap(task_pixmap.scaled(40, 40));
    task_layout->addWidget(task_icon);

    //name
    QCustomLabel* task_name = new QCustomLabel(task.text);
    task_name->setMargin(5);
    task_layout->addWidget(task_name);

    QSpacerItem* spacer = new QSpacerItem(80, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    task_layout->addSpacerItem(spacer);

    //"create daytask" button
    IconButton* add_task_btn = new IconButton(QIcon(":/Images/Resources/ic_add_black_24px.svg"));
    connect(add_task_btn, &QPushButton::clicked, [this, task] {
        current_task = task;
        ui->stackedWidget->setCurrentIndex(1);
    });
    task_layout->addWidget(add_task_btn);

    //"edit task" button
    IconButton* edit_task_btn = new IconButton(QIcon(":/Images/Resources/ic_mode_edit_black_24px.svg"));
    task_layout->addWidget(edit_task_btn);

    //"delete task" button
    IconButton* delete_task_btn = new IconButton(QIcon(":/Images/Resources/ic_delete_black_24px.svg"));
//    connect(delete_task_btn, &QPushButton::clicked, [this, task] {
//        if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this task? Every day task associated with it will be permanently deleted.", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
//
//        }
//        current_task = task;

//    });
    task_layout->addWidget(delete_task_btn);

    return task_widget;
}

void TaskDialog::addDayTask()
{
///TODO: SANITIZE USER INPUT
    QTime attempted_end_time(ui->startTime->time());
    attempted_end_time = attempted_end_time.addSecs(ui->duration->time().hour() * 60 * 60);
    attempted_end_time = attempted_end_time.addSecs(ui->duration->time().minute() * 60);

    DayTaskController::addDayTask(current_date, DayTask(current_task.id, ui->startTime->time(), attempted_end_time));

    emit dayTaskAdded();
}

void TaskDialog::addTask()
{
///TODO: SANITIZE USER INPUT
    task_container.addTask(Task {
                               ui->taskNameEdit->text(),                                        //text
                               group_container.getGroup(ui->taskGroupBox->currentIndex()).id,   //group_id
                               ui->taskIconBox->currentText()                                   //icon_path
                           });
}

void TaskDialog::initConnects()
{
    connect(ui->closeBtn, &QPushButton::clicked, [this] { this->close(); });
    connect(ui->cancelBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->addDayTaskBtn, &QPushButton::clicked, [this] {
        addDayTask();
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->cancelAddTaskBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->addTaskBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(2); });
    connect(ui->confirmAddTaskBtn, &QPushButton::clicked, [this] {
        addTask();
        drawTasks();
        ui->stackedWidget->setCurrentIndex(0);
    });

}


TaskDialog::~TaskDialog()
{
    delete ui;
}

