#include "taskdialog.h"
#include "ui_taskdialog.h"
#include "models/task.h"
#include "helpers/iconmanager.h"
#include "custom_widgets/qcustomlabel.h"
#include "custom_widgets/iconbutton.h"
#include "custom_widgets/materialdialog.h"
#include "controllers/datecontrollers.h"
#include <QMessageBox>
#include <QScrollArea>
#include <QFormLayout>
#include <QTimeEdit>
#include <QComboBox>

TaskDialog::TaskDialog(QDate current_date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskAddDialog),
    icon_mng(new IconManager),
    current_date{current_date}
{
    ui->setupUi(this);

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
    QCustomLabel* task_name = new QCustomLabel(task.name);
    task_name->setMargin(5);
    task_layout->addWidget(task_name);

    QSpacerItem* spacer = new QSpacerItem(10, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    task_layout->addSpacerItem(spacer);

    //"create daytask" button
    IconButton* add_task_btn = new IconButton(QIcon(":/Images/Resources/ic_add_black_24px.svg"));
    //prompt a dialog on click
    connect(add_task_btn, &QPushButton::clicked, [this, task] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();
        main_layout->setContentsMargins(24, 26, 24, 24);
        main_layout->setHorizontalSpacing(8);
        main_layout->setVerticalSpacing(6);

        QLabel* start_time_lbl = new QLabel("Start time", material_dlg);
        start_time_lbl->setFont(QFont("Roboto Medium", 10));
        start_time_lbl->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        QTimeEdit* start_time_edit = new QTimeEdit(QDateTime::currentDateTime().time(), material_dlg);
        start_time_edit->setFont(QFont("Roboto Medium", 10));
        start_time_edit->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        start_time_edit->setDisplayFormat("hh:mm");

        main_layout->addRow(start_time_lbl, start_time_edit);

        QLabel* duration_lbl = new QLabel("Duration", material_dlg);
        duration_lbl->setFont(QFont("Roboto Medium", 10));
        duration_lbl->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        QTimeEdit* duration_edit = new QTimeEdit(material_dlg);
        duration_edit->setFont(QFont("Roboto Medium", 10));
        duration_edit->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        duration_edit->setDisplayFormat("hh:mm");

        main_layout->addRow(duration_lbl, duration_edit);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("New task");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("ADD TASK");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, task, start_time_edit, duration_edit] {
            addDayTask(task.id, start_time_edit->time(), duration_edit->time());
            material_dlg->close();
        });
        material_dlg->show();
    });
    task_layout->addWidget(add_task_btn);

    //"edit task" button
    IconButton* edit_task_btn = new IconButton(QIcon(":/Images/Resources/ic_mode_edit_black_24px.svg"));
    task_layout->addWidget(edit_task_btn);

    //"delete task" button
    IconButton* delete_task_btn = new IconButton(QIcon(":/Images/Resources/ic_delete_black_24px.svg"));
    connect(delete_task_btn, &QPushButton::clicked, [this, task] {
        if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this task? Every day task associated with it will be permanently deleted.", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
           task_container.removeTask(task.id);
           drawTasks();
           emit changed();
        }
    });
    task_layout->addWidget(delete_task_btn);

    return task_widget;
}

void TaskDialog::addDayTask(int task_id, QTime start_time, QTime duration)
{
///TODO: SANITIZE USER INPUT
    QTime attempted_end_time(start_time);
    attempted_end_time = attempted_end_time.addSecs(duration.hour() * 60 * 60);
    attempted_end_time = attempted_end_time.addSecs(duration.minute() * 60);

    DayTaskController::addDayTask(current_date, DayTask(task_id, start_time, attempted_end_time));

    emit changed();
}

void TaskDialog::addTask(QString icon_path, QString name, int group_id)
{
///TODO: SANITIZE USER INPUT
    task_container.addTask(Task{ name, group_id, icon_path });

    drawTasks();
}

void TaskDialog::initConnects()
{
    connect(ui->closeBtn, &QPushButton::clicked, [this] { this->close(); });
    connect(ui->addTaskBtn, &QPushButton::clicked, [this] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();
        main_layout->setContentsMargins(24, 26, 24, 24);
        main_layout->setHorizontalSpacing(8);
        main_layout->setVerticalSpacing(6);

        QLabel* icon_lbl = new QLabel("Icon", material_dlg);
        icon_lbl->setFont(QFont("Roboto Medium", 10));
        icon_lbl->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        QComboBox* icon_box = new QComboBox(material_dlg);
        icon_box->setFont(QFont("Roboto Medium", 10));
        icon_box->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        icon_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        //fill %icon_box with icons from /icons folder
        QStringList icon_list = icon_mng->getIconList();
        for (int i = 0; i < icon_list.count(); i++) {
            icon_box->addItem(QIcon(icon_list[i]), icon_list[i]);
        }

        main_layout->addRow(icon_lbl, icon_box);

        QLabel* name_lbl = new QLabel("Name", material_dlg);
        name_lbl->setFont(QFont("Roboto Medium", 10));
        name_lbl->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        QLineEdit* name_edit = new QLineEdit(material_dlg);
        name_edit->setFont(QFont("Roboto Medium", 10));
        name_edit->setStyleSheet("color: rgba(0, 0, 0,  66%);");

        main_layout->addRow(name_lbl, name_edit);

        QLabel* group_lbl = new QLabel("Icon", material_dlg);
        group_lbl->setFont(QFont("Roboto Medium", 10));
        group_lbl->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        QComboBox* group_box = new QComboBox(material_dlg);
        group_box->setFont(QFont("Roboto Medium", 10));
        group_box->setStyleSheet("color: rgba(0, 0, 0,  66%);");
        group_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
        //fill %group_box with groups
        for (int i = 0; i < group_container.groupCount(); i++) {
            group_box->addItem(group_container.getGroup(i).name);
        }

        main_layout->addRow(group_lbl, group_box);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("New task template");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("ADD TEMPLATE");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, icon_box, name_edit, group_box] {
            addTask(icon_box->currentText(), name_edit->text(), group_box->currentIndex());
            material_dlg->close();
        });
        material_dlg->show();
    });
}


TaskDialog::~TaskDialog()
{
    delete ui;
}

