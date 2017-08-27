#include "taskwidget.h"
#include "custom_widgets/qcustomlabel.h"
#include "custom_widgets/taskprogressbar.h"
#include "custom_widgets/mycustomshadoweffect.h"
#include "custom_widgets/myiconbutton.h"
#include "custom_widgets/materialdialog.h"
#include "controllers/tasktemplatecontainer.h"
#include "controllers/groupcontainer.h"
#include <QMenu>
#include <QMessageBox>
#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QTimeEdit>
#include <QComboBox>
#include <QTimer>
#include <QPalette>

TaskWidget::TaskWidget(QDate date, Task task, QWidget *parent) :
    QWidget(parent),
    current_date{date}
{
    //Assemble task widget
    //Status and icon
        //status
        QFrame* status_color = new QFrame(this);
        status_color->setFrameShape(QFrame::VLine);
        status_color->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
        status_color->setMinimumWidth(4);
        status_color->setAutoFillBackground(true);
        QString style = "background: %1; border: 0px;";
        switch (task.is_done) {
        case 0:
            style = style.arg(constants::red);
            break;
        case 1:
            style = style.arg(constants::green);
            break;
    //    case 2:
    //        style = style.arg(constants::red);
    //        break;
        }
        status_color->setStyleSheet(style);

        //... and icon
        QCustomLabel* icon_lbl = new QCustomLabel(this);
        QPixmap icon(tasktemplate_container.findTaskTemplate(task.tasktemplate_id).icon_path);
        icon_lbl->setPixmap(icon.scaled(QSize(60,60)));
    //Labels
        //task name
        QCustomLabel* text_lbl = new QCustomLabel(tasktemplate_container.findTaskTemplate(task.tasktemplate_id).name, this);
        text_lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        text_lbl->setFont(QFont("Roboto", 11, 75, false));
        //group name
        QCustomLabel* group_lbl = new QCustomLabel(tasktemplate_container.getGroup(task.tasktemplate_id).name.toLower(), this);
        group_lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        group_lbl->setFont(QFont("Roboto", 10, 75, false));
        group_lbl->setStyleSheet("color: #ffffff;"
                                 "border: 1px solid;"
                                 "border-color:" + tasktemplate_container.getGroup(task.tasktemplate_id).color + ";"
                                 "background:" + tasktemplate_container.getGroup(task.tasktemplate_id).color + ";"
                                 "border-radius: 4px;");
        //start time
        QCustomLabel* start_time_lbl = new QCustomLabel(task.start_time.toString("HH:mm"), this);
        start_time_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        start_time_lbl->setFont(QFont("Roboto", 9, 50, false));
        start_time_lbl->setObjectName("start_time_lbl");
        //separator
        QCustomLabel* time_separator_lbl = new QCustomLabel("-", this);
        time_separator_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        time_separator_lbl->setFont(QFont("Roboto", 9, 50, false));
        //end time
        QCustomLabel* end_time_lbl = new QCustomLabel(TaskController::getEndTime(current_date, task.id).toString("HH:mm"), this);
        end_time_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        end_time_lbl->setFont(QFont("Roboto", 9, 50, false));
        end_time_lbl->setObjectName("end_time_lbl");
        //separator
        QCustomLabel* duration_separator_lbl = new QCustomLabel(" | ", this);
        duration_separator_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        duration_separator_lbl->setFont(QFont("Roboto", 11, 50, false));
        //duration
        QCustomLabel* duration_lbl = new QCustomLabel(this);
        duration_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
        duration_lbl->setFont(QFont("Roboto", 9, 50, true));
        duration_lbl->setText(QString::number(task.duration.hour())
                              + " hour(s) " + QString::number(task.duration.minute())
                              + " minute(s)");
    //Buttons
        //more
        MyIconButton* more_btn = new MyIconButton(QIcon(":/Images/Resources/ic_more_vert_black_24px.svg"), this);
        //context menu and actions
        //!!!!!!!!! %context_menu has to have no parent, otherwise can't call %drawtasks() when %remove_task is clicked
        QMenu* context_menu = new QMenu();
        //delete task
        QAction* remove_task = new QAction("Delete", context_menu);
        remove_task->setFont(QFont("Roboto", 9, 50, false));
        connect(remove_task, &QAction::triggered, [this, task] {
            if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this task?", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
                    TaskController::removeTask(current_date, task.id);
                    emit redrawNeeded();
            }
        });
    //    QAction* view_breaks = new QAction("View Breaks", context_menu);
    //    view_breaks->setFont(QFont("Roboto", 9, 50, false));
    //    connect(view_breaks, &QAction::triggered, [this, i] {
    //        break_window = new BreakWindow(i, date()[g::y][g::m][g::d].getTask(i).startTime(), date()[g::y][g::m][g::d].getTask(i).endTime());
    //        connect(break_window, &BreakWindow::breakAdded, [this] { drawtasks(); });
    //        break_window->show();
    //    });
        //edit task time
        QAction* edit_task_time = new QAction("Edit time", context_menu);
        edit_task_time->setFont(QFont("Roboto", 9, 50, false));
        connect(edit_task_time, &QAction::triggered, [this, task] {
            //material dialog template
            MaterialDialog* material_dlg = new MaterialDialog(this);

            //form a layout of controls
            QFormLayout* main_layout = new QFormLayout();

            QLabel* start_time_lbl = new QLabel("Start time", material_dlg);
            QTimeEdit* start_time_edit = new QTimeEdit(QDateTime::currentDateTime().time(), material_dlg);
            start_time_edit->setDisplayFormat("hh:mm");
            start_time_edit->setTime(task.start_time);

            main_layout->addRow(start_time_lbl, start_time_edit);

            QLabel* duration_lbl = new QLabel("Duration", material_dlg);
            QTimeEdit* duration_edit = new QTimeEdit(material_dlg);
            duration_edit->setDisplayFormat("hh:mm");
            duration_edit->setTime(task.duration);

            main_layout->addRow(duration_lbl, duration_edit);

            //add controls to the dialog
            material_dlg->insertLayout(main_layout);
            material_dlg->setHeading("Edit task time");
            material_dlg->setCancelBtnText("CANCEL");
            material_dlg->setConfirmBtnText("CONFIRM");

            connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, task, start_time_edit, duration_edit] {
                editTaskTime(task.id, start_time_edit->time(), duration_edit->time());
                material_dlg->close();
                if (redrawIsNeeded)
                    emit redrawNeeded();
            });
            material_dlg->show();
        });
        //mark as done
        QAction* mark_done = new QAction(context_menu);
        mark_done->setFont(QFont("Roboto", 9, 50, false));
        switch (task.is_done) {
        case 0:
            mark_done->setText("Mark done");
            connect(mark_done, &QAction::triggered, [this, task] {
                TaskController::setStatus(current_date, task.id, 1);
                emit redrawNeeded();
            });
            break;
        case 1:
            mark_done->setText("Mark undone");
            connect(mark_done, &QAction::triggered, [this, task] {
                TaskController::setStatus(current_date, task.id, 0);
                emit redrawNeeded();
            });
            break;
    //    case 2:
    //        mark_done->setText("Mark done");
    //        connect(mark_done, &QAction::triggered, [this, task] {
    //            int secs = task.start_time.secsTo(QTime::currentTime());
    //            QTime duration(0,0,0,0);
    //            duration.addSecs(secs);
    //            TaskController::setDuration(current_date, task.id, duration);
    //            TaskController::setStatus(current_date, task.id, 1);
    //            drawtasks();
    //        });
    //        break;
        }

        context_menu->addAction(mark_done);
    //    context_menu->addAction(view_breaks);
        context_menu->addAction(edit_task_time);
        context_menu->addAction(remove_task);
        connect(more_btn, &QPushButton::clicked, [context_menu, more_btn] {
            context_menu->popup(more_btn->mapToGlobal(QPoint(-81,21)));
        });
    //    //Note preview
    //        ClickableLabel* note = new ClickableLabel("No note", task_widget_vec[i]);
    //        if (date()[g::y][g::m][g::d].getTask(i).note() != "") {
    //            QString text = date()[g::y][g::m][g::d].getTask(i).note();
    //            text.replace("\n", "<br>");
    //            note->setHtml(text);
    //        }
    //        connect(note, &ClickableLabel::clicked, [this, i]() {
    //            daynote_view = std::make_pair(false, false);
    //            ui->textEdit->setPlainText(date()[g::y][g::m][g::d].getTask(i).note());
    //            ui->textBrowser->setHtml(date()[g::y][g::m][g::d].getTask(i).note());
    //            current_task_index = i;
    //            ui->stackedWidget->setCurrentIndex(2);
    //        });
    //        note->setFont(QFont("Roboto", 8, 50, true));
    //        note->setReadOnly(true);
    //        note->viewport()->setCursor(QCursor(Qt::PointingHandCursor));
    //        note->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    //        note->setAlignment(Qt::AlignVCenter);
    //        note->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //        note->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //        note->setStyleSheet("ClickableLabel {"
    //                            "border: 1px solid rgb(189, 189, 189);"
    //                            "border-right: 0px; border-top: 0px; border-bottom: 0px;"
    //                            "color: #9e9e9e;"
    //                            "}");
    //Layouts
        QHBoxLayout* top_layout = new QHBoxLayout();
        top_layout->setContentsMargins(0, 0, 0, 0);
        top_layout->setSpacing(16);
        top_layout->addWidget(text_lbl);
        top_layout->addWidget(group_lbl);
        QSpacerItem* first_spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding);
        top_layout->addSpacerItem(first_spacer);
        top_layout->addWidget(more_btn);

        QHBoxLayout* time_layout = new QHBoxLayout();
        time_layout->setContentsMargins(0, 0, 0, 0);
        time_layout->setSpacing(3);
        time_layout->addWidget(start_time_lbl);
        time_layout->addWidget(time_separator_lbl);
        time_layout->addWidget(end_time_lbl);
        time_layout->addWidget(duration_separator_lbl);
        time_layout->addWidget(duration_lbl);

        QHBoxLayout* middle_layout = new QHBoxLayout();
        middle_layout->setContentsMargins(0, 0, 0, 0);
        middle_layout->setSpacing(12);
        middle_layout->addLayout(time_layout);
        QSpacerItem* second_spacer = new QSpacerItem(20, 20, QSizePolicy::Expanding);
        middle_layout->addSpacerItem(second_spacer);

        QHBoxLayout* bottom_layout = new QHBoxLayout();
        bottom_layout->setContentsMargins(0, 0, 0, 0);
        bottom_layout->setSpacing(12);
        //Add either timeline or break info to the layout
        if (current_date == QDate::currentDate() &&
                task.start_time <= QTime::currentTime() &&
                TaskController::getEndTime(current_date, task.id) > QTime::currentTime()) {
            TaskProgressBar* bar = new TaskProgressBar(current_date, task, this);
            QTimer* timer = new QTimer(this);
            connect(timer, &QTimer::timeout, [this, bar]() {
                bar->progress();
            });
            connect(bar, &TaskProgressBar::finished, [this, timer]() {
                delete timer;
                emit redrawNeeded();
            });
            timer->start(60000);
            bottom_layout->addWidget(bar);
        } else {
            QCustomLabel* break_lbl;
            if (BreakController::breakCount(current_date, task.id) > 0) {
                int duration_sum = 0;
                for (size_t j = 0; j < BreakController::breakCount(current_date, task.id); j++) {
                    QTime start_time = BreakController::getBreak(current_date, task.id, j).start_time;
                    QTime end_time = BreakController::getBreak(current_date, task.id, j).end_time;
                    duration_sum += start_time.secsTo(end_time);
                }
                int minute = (duration_sum / 60) % 60;
                duration_sum = (duration_sum / 60) / 60;
                break_lbl = new QCustomLabel(QString::number(BreakController::breakCount(current_date, task.id)) + " break(s): " +
                                       QString::number(duration_sum) + " hour(s) and " +
                                       QString::number(minute) + " minute(s) total", this);
            } else {
                break_lbl = new QCustomLabel("No breaks", this);
            }
            break_lbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
            break_lbl->setFont(QFont("Roboto", 9, 50 , true));

            bottom_layout->addWidget(break_lbl);
        }

        QVBoxLayout* widget_layout = new QVBoxLayout();
        widget_layout->setContentsMargins(9, 0, 0, 0);
        widget_layout->setSpacing(6);
        widget_layout->addLayout(top_layout);
        widget_layout->addLayout(middle_layout);
        widget_layout->addLayout(bottom_layout);

        QHBoxLayout* left_layout = new QHBoxLayout();
        left_layout->setContentsMargins(0, 9, 9, 9);
        left_layout->setSpacing(6);
        left_layout->addWidget(icon_lbl);
        left_layout->addLayout(widget_layout);

    //        QHBoxLayout* right_layout = new QHBoxLayout();
    //        right_layout->setContentsMargins(0, 6, 0, 6);
    //        right_layout->addWidget(note);

        QHBoxLayout* task_layout = new QHBoxLayout();
        task_layout->setContentsMargins(0, 0, 0, 0);
        task_layout->addWidget(status_color);
        task_layout->addLayout(left_layout);
        //task_layout->addLayout(right_layout);
        task_layout->setStretch(1, 7);
        task_layout->setStretch(2, 3);

    //Add everything to task widget
        this->setAutoFillBackground(true);
        QPalette widget_palette;
        widget_palette.setColor(QPalette::Window, QColor(255, 255, 255));
        this->setPalette(widget_palette);
        this->setFixedHeight(84);
        this->setLayout(task_layout);
        this->setStyleSheet(".QCustomLabel { border: 0px; }");
        //shadow effect
        MyCustomShadowEffect *bodyShadow = new MyCustomShadowEffect();
        bodyShadow->setBlurRadius(6.0);
        bodyShadow->setDistance(2.0);
        bodyShadow->setColor(QColor(0, 0, 0, 20));
        this->setGraphicsEffect(bodyShadow);
}

void TaskWidget::editTaskTime(int task_id, QTime start_time, QTime duration)
{
    TaskController::setStartTime(current_date, task_id, start_time);
    TaskController::setDuration(current_date, task_id, duration);

    redrawIsNeeded = true;
}
