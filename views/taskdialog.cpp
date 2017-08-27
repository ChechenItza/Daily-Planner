#include "taskdialog.h"
#include "ui_taskdialog.h"
#include "models/tasktemplate.h"
#include "helpers/iconmanager.h"
#include "custom_widgets/qcustomlabel.h"
#include "custom_widgets/myiconbutton.h"
#include "custom_widgets/materialdialog.h"
#include "controllers/datecontrollers.h"
#include "controllers/groupcontainer.h"
#include "controllers/tasktemplatecontainer.h"
#include <QMessageBox>
#include <QScrollArea>
#include <QFormLayout>
#include <QTimeEdit>
#include <QComboBox>
#include <QLineEdit>

TaskDialog::TaskDialog(QDate current_date, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TaskAddDialog),
    icon_mng(new IconManager),
    current_date{current_date}
{
    ui->setupUi(this);

    initConnects();
    drawTasks();
    drawGroups();
}

void TaskDialog::drawTasks()
{
    //save current index
    int index;
    if (ui->tabWidget->count() != 0)
        index = ui->tabWidget->currentIndex();
    else
        index = 0;

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
        for (int j = 0; j < tasktemplate_container.taskCount(); j++) {
            if (tasktemplate_container.getTaskTemplate(j).group_id == group_container.getGroup(i).id)
                tab_layout->addWidget(genTask(tasktemplate_container.getTaskTemplate(j)));
        }
        QSpacerItem* task_spacer = new QSpacerItem(40, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
        tab_layout->addSpacerItem(task_spacer);

        //add page to ui
        ui->tabWidget->addTab(tab_widget_scroll, group_container.getGroup(i).name);
    }

    ui->tabWidget->setCurrentIndex(index);
}

QWidget* TaskDialog::genTask(TaskTemplate task)
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

    //"create Task" button
    MyIconButton* add_task_btn = new MyIconButton(QIcon(":/Images/Resources/ic_add_black_24px.svg"));
    //prompt a dialog on click
    connect(add_task_btn, &QPushButton::clicked, [this, task] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* start_time_lbl = new QLabel("Start time", material_dlg);
        QTimeEdit* start_time_edit = new QTimeEdit(QDateTime::currentDateTime().time(), material_dlg);
        start_time_edit->setDisplayFormat("hh:mm");

        main_layout->addRow(start_time_lbl, start_time_edit);

        QLabel* duration_lbl = new QLabel("Duration", material_dlg);
        QTimeEdit* duration_edit = new QTimeEdit(material_dlg);
        duration_edit->setDisplayFormat("hh:mm");

        main_layout->addRow(duration_lbl, duration_edit);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("New task");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("ADD NEW");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, task, start_time_edit, duration_edit] {
            addTask(task.id, start_time_edit->time(), duration_edit->time());
            material_dlg->close();
        });
        material_dlg->show();
    });
    task_layout->addWidget(add_task_btn);

    //"edit task" button
    MyIconButton* edit_task_btn = new MyIconButton(QIcon(":/Images/Resources/ic_mode_edit_black_24px.svg"));
    connect(edit_task_btn, &MyIconButton::clicked, [this, task] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* icon_lbl = new QLabel("Icon", material_dlg);
        QComboBox* icon_box = new QComboBox(material_dlg);
        icon_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        //fill %icon_box with icons from /icons folder
        QStringList icon_list = icon_mng->getIconList();
        for (int i = 0; i < icon_list.count(); i++) {
            icon_box->addItem(QIcon(icon_list[i]), icon_list[i]);
        }
        //set it to task's icon
        for (int i = 0; i < icon_box->count(); i++) {
            icon_box->setCurrentIndex(i);
            if (icon_box->currentText() == task.icon_path)
                break;
        }

        main_layout->addRow(icon_lbl, icon_box);

        QLabel* name_lbl = new QLabel("Name", material_dlg);
        QLineEdit* name_edit = new QLineEdit(task.name ,material_dlg);

        main_layout->addRow(name_lbl, name_edit);

        QLabel* group_lbl = new QLabel("Group", material_dlg);
        QComboBox* group_box = new QComboBox(material_dlg);
        group_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
        //fill %group_box with groups
        for (int i = 0; i < group_container.groupCount(); i++) {
            group_box->addItem(group_container.getGroup(i).name);
        }
        //set it to task's group
        for (int i = 0; i < group_container.groupCount(); i++) {
            if (group_container.getGroup(i).id == task.group_id) {
                group_box->setCurrentIndex(i);
                break;
            }
        }

        main_layout->addRow(group_lbl, group_box);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("Edit task template");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("CONFIRM");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, task, material_dlg, icon_box, name_edit, group_box] {
            editTask(task.id, icon_box->currentText(), name_edit->text(), group_box->currentIndex());
            material_dlg->close();
        });
        material_dlg->show();
    });
    task_layout->addWidget(edit_task_btn);

    //"delete task" button
    MyIconButton* delete_task_btn = new MyIconButton(QIcon(":/Images/Resources/ic_delete_black_24px.svg"));
    connect(delete_task_btn, &QPushButton::clicked, [this, task] {
        if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this task? Every day task associated with it will be permanently deleted.", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
           tasktemplate_container.removeTaskTemplate(task.id);
           drawTasks();
           emit changed();
        }
    });
    task_layout->addWidget(delete_task_btn);

    return task_widget;
}

void TaskDialog::addTask(int task_id, QTime start_time, QTime duration)
{
///TODO: SANITIZE USER INPUT
    TaskController::addTask(current_date, Task(task_id, start_time, duration));

    emit changed();
}

void TaskDialog::addTask(QString icon_path, QString name, int group_id)
{
///TODO: SANITIZE USER INPUT
    tasktemplate_container.addTaskTemplate(TaskTemplate{ name, group_id, icon_path });

    drawTasks();
}

void TaskDialog::editTask(int task_id, QString icon_path, QString name, int group_id)
{
    tasktemplate_container.setIconPath(task_id, icon_path);
    tasktemplate_container.setName(task_id, name);
    tasktemplate_container.setGroup(task_id, group_id);

    drawTasks();
    emit changed();
}

void TaskDialog::addGroup(QString name, QString color)
{
    group_container.addGroup(Group{ name, color });
    drawTasks();
}

void TaskDialog::drawGroups()
{
    //clear groups
    while (ui->groupSelectLayout->count() != 0) {
        delete ui->groupSelectLayout->itemAt(0)->widget();
    }

    //page
    QScrollArea* group_scroll = new QScrollArea(this);
    group_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    group_scroll->setWidgetResizable(true);
    group_scroll->setFrameShape(QFrame::Shape::NoFrame);
    group_scroll->setStyleSheet("QScrollArea { "
                                     "border-bottom: 1px solid #d2d2d2; "
                                     "border-top: 0px;"
                                     "border-right: 0px;"
                                     "border-left: 0px; }");
    QWidget* group_widget = new QWidget();
    group_widget->setStyleSheet("background-color: #fafafa;");
    group_scroll->setWidget(group_widget);

    QVBoxLayout* group_layout = new QVBoxLayout(group_widget);
    group_layout->setSpacing(0);
    group_layout->setContentsMargins(0, 0, 0, 0);

    //add task widgets to it
    for (int j = 0; j < group_container.groupCount(); j++) {
        group_layout->addWidget(genGroup(group_container.getGroup(j)));
    }
    QSpacerItem* task_spacer = new QSpacerItem(40, 40, QSizePolicy::Expanding, QSizePolicy::Expanding);
    group_layout->addSpacerItem(task_spacer);

    //add page to ui
    ui->groupSelectLayout->addWidget(group_scroll);
}

QWidget* TaskDialog::genGroup(Group group)
{
    //container
    QWidget* group_widget = new QWidget();
    group_widget->setStyleSheet(".QWidget { border-bottom: 1px solid #d2d2d2; }");
    //container layout
    QHBoxLayout* group_layout = new QHBoxLayout(group_widget);
    group_layout->setSpacing(6);

    //group name and color
    QCustomLabel* group_lbl = new QCustomLabel(group.name.toLower());
    group_lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    group_lbl->setFont(QFont("Roboto", 10, 75, false));
    group_lbl->setStyleSheet("color: #ffffff;"
                             "border: 1px solid;"
                             "border-color:" + group.color + ";"
                             "background:" + group.color + ";"
                             "border-radius: 4px;");
    group_layout->addWidget(group_lbl);

    QSpacerItem* spacer = new QSpacerItem(10, 40, QSizePolicy::Expanding, QSizePolicy::Minimum);
    group_layout->addSpacerItem(spacer);

    //"edit task" button
    MyIconButton* edit_group_btn = new MyIconButton(QIcon(":/Images/Resources/ic_mode_edit_black_24px.svg"));
    connect(edit_group_btn, &MyIconButton::clicked, [this, group] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* name_lbl = new QLabel("Name", material_dlg);
        QLineEdit* name_edit = new QLineEdit(group.name, material_dlg);

        main_layout->addRow(name_lbl, name_edit);

        QLabel* color_lbl = new QLabel("Color", material_dlg);
        QComboBox* color_box = new QComboBox(material_dlg);
        color_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        for (QString color : constants::color_arr) {
            QPixmap pixmap(100, 100);
            pixmap.fill(QColor(color));
            color_box->addItem(QIcon(pixmap), color);
        }
        for (int i = 0; i < color_box->count(); i++) {
            if (color_box->itemText(i) == group.color) {
                color_box->setCurrentIndex(i);
                break;
            }
        }

        main_layout->addRow(color_lbl, color_box);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("Edit group");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("CONFIRM");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, group, material_dlg, name_edit, color_box] {
            group_container.setName(group.id, name_edit->text());
            group_container.setColor(group.id, color_box->currentText());
            drawGroups();
            drawTasks();
            emit changed();
            material_dlg->close();
        });
        material_dlg->show();
    });
    group_layout->addWidget(edit_group_btn);

    //"delete task" button
    MyIconButton* delete_group_btn = new MyIconButton(QIcon(":/Images/Resources/ic_delete_black_24px.svg"));
    connect(delete_group_btn, &QPushButton::clicked, [this, group] {
        if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this group? Every task template associated with it will be permanently deleted.", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
           group_container.removeGroup(group.id);
           drawGroups();
           drawTasks();
           emit changed();
        }
    });
    group_layout->addWidget(delete_group_btn);

    return group_widget;
}

void TaskDialog::initConnects()
{
    connect(ui->closeBtn, &QPushButton::clicked, [this] { this->close(); });
    connect(ui->closeBtn_2, &QPushButton::clicked, [this] { this->close(); });
    connect(ui->mngGroupsBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(1); });
    connect(ui->mngTasksBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->addTaskBtn, &QPushButton::clicked, [this] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* icon_lbl = new QLabel("Icon", material_dlg);

        QComboBox* icon_box = new QComboBox(material_dlg);
        icon_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        //fill %icon_box with icons from /icons folder
        QStringList icon_list = icon_mng->getIconList();
        for (int i = 0; i < icon_list.count(); i++) {
            icon_box->addItem(QIcon(icon_list[i]), icon_list[i]);
        }

        main_layout->addRow(icon_lbl, icon_box);

        QLabel* name_lbl = new QLabel("Name", material_dlg);
        QLineEdit* name_edit = new QLineEdit(material_dlg);

        main_layout->addRow(name_lbl, name_edit);

        QLabel* group_lbl = new QLabel("Group", material_dlg);
        QComboBox* group_box = new QComboBox(material_dlg);
        group_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
        //fill %group_box with groups
        for (int i = 0; i < group_container.groupCount(); i++) {
            group_box->addItem(group_container.getGroup(i).name);
        }
        group_box->setCurrentIndex(ui->tabWidget->currentIndex());

        main_layout->addRow(group_lbl, group_box);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("New task template");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("ADD NEW");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, icon_box, name_edit, group_box] {
            addTask(icon_box->currentText(), name_edit->text(), group_box->currentIndex());
            material_dlg->close();
        });
        material_dlg->show();
    });

    connect(ui->addGroupBtn, &QPushButton::clicked, [this] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* name_lbl = new QLabel("Name", material_dlg);
        QLineEdit* name_edit = new QLineEdit(material_dlg);

        main_layout->addRow(name_lbl, name_edit);

        QLabel* color_lbl = new QLabel("Color", material_dlg);
        QComboBox* color_box = new QComboBox(material_dlg);
        color_box->setSizeAdjustPolicy(QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLengthWithIcon);
        for (QString color : constants::color_arr) {
            QPixmap pixmap(100, 100);
            pixmap.fill(QColor(color));
            color_box->addItem(QIcon(pixmap), color);
        }

        main_layout->addRow(color_lbl, color_box);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("New group");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("ADD NEW");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, name_edit, color_box] {
            addGroup(name_edit->text(), color_box->currentText());
            drawGroups();
            material_dlg->close();
        });
        material_dlg->show();
    });
}


TaskDialog::~TaskDialog()
{
    delete ui;
}

