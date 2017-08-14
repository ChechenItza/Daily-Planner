#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDate"
#include "settings/constants.h"
#include "controllers/db.h"
#include "controllers/datecontrollers.h"
#include "custom_widgets/qcustomlabel.h"
#include "custom_widgets/taskprogressbar.h"
#include "custom_widgets/mycustomshadoweffect.h"
#include "views/taskdialog.h"
#include "custom_widgets/myiconbutton.h"
#include "custom_widgets/materialdialog.h"
#include "custom_widgets/material_widgets/xx/qtmaterialselectfield.h"
#include <QMenu>
#include <QMessageBox>
#include <algorithm>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStyle>
#include <QDesktopWidget>
#include <QFormLayout>
#include <QTimeEdit>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    current_date(QDate::currentDate())
{
    ui->setupUi(this);
    db;
    initConnects();
    redrawDate();
    drawDayButtons();

    //center the window
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
        )
    );

    QtMaterialSelectField *sfp = new QtMaterialSelectField;
    sfp->setPlaceholderText("Favorite language");
    //sfp->setBackgroundColor(Qt::white);
    ui->verticalLayout_9->addWidget(sfp);
    {
        sfp->addItem("C");
        sfp->addItem("C++");
        sfp->addItem("Haskell");
        sfp->addItem("JavaScript");
        sfp->addItem("ECMAScript");
        sfp->addItem("OCaml");
        sfp->addItem("Python");
        sfp->addItem("F#");
        sfp->addItem("Clojure");
        sfp->addItem("Java");
    }

//shadow effect for stats window elements
//    CustomShadowEffect *body_shadow_1 = new CustomShadowEffect();
//    body_shadow_1->setBlurRadius(6.0);
//    body_shadow_1->setDistance(2.0);
//    body_shadow_1->setColor(QColor(0, 0, 0, 20));
//    ui->frame_1->setGraphicsEffect(body_shadow_1);
//    CustomShadowEffect *body_shadow_2 = new CustomShadowEffect();
//    body_shadow_2->setBlurRadius(6.0);
//    body_shadow_2->setDistance(2.0);
//    body_shadow_2->setColor(QColor(0, 0, 0, 20));
//    ui->frame_2->setGraphicsEffect(body_shadow_2);
//    CustomShadowEffect *body_shadow_3 = new CustomShadowEffect();
//    body_shadow_3->setBlurRadius(6.0);
//    body_shadow_3->setDistance(2.0);
//    body_shadow_3->setColor(QColor(0, 0, 0, 20));
//    ui->frame_3->setGraphicsEffect(body_shadow_3);
}

void MainWindow::openTaskDlg()
{
    task_dlg = new TaskDialog(current_date, this);
    task_dlg->setModal(true);
    task_dlg->setAttribute(Qt::WA_DeleteOnClose);
    connect(task_dlg, &TaskDialog::changed, [this] { drawDayTasks(); });

    task_dlg->show();
}


void MainWindow::drawDayButtons()
{
//Delete all previous day button widgets in %daybutton_vec
    if (!daybutton_vec.empty()) {
        std::for_each(daybutton_vec.begin(), daybutton_vec.end(), [](QPushButton* btn) {delete btn;});
        daybutton_vec.clear();
    }

//Copy current date and set day to 1
    QDate month_date;
    month_date.setDate(current_date.year(), current_date.month(), 1);

//Fill %daybutton_vec with day buttons in 3 steps
    //fill it with empty buttons until the first weekday of the month
    for (int i = 1; i < month_date.dayOfWeek(); i++) {
        daybutton_vec.push_back(genEmptyDayBtn());
    }

    //fill it with normal buttons until the last weekday of the month
    QDate tmp_date;
    for (int i = month_date.dayOfWeek(); i < month_date.daysInMonth() + month_date.dayOfWeek(); i++) {
        tmp_date.setDate(month_date.year(), month_date.month(), i + 1 - month_date.dayOfWeek());
        daybutton_vec.push_back(genDayBtn(tmp_date));
    }

    //fill the rest with empty buttons
    for (int i = month_date.daysInMonth() + month_date.dayOfWeek(); i < 42 + 1; i++) {
        daybutton_vec.push_back(genEmptyDayBtn());
    }

//Add day button widgets to ui
    int d = 0;
    for (int y = 0; y < 6; y++) {
        for (int x = 0; x < 7; x++) {
            ui->dayButtonLayout->addWidget(daybutton_vec[d], y, x);
            if (x == 6)
                daybutton_vec[d]->setStyleSheet(daybutton_vec[d]->styleSheet() + "QPushButton{border-right: 0px;}");
            if (y == 5)
                daybutton_vec[d]->setStyleSheet(daybutton_vec[d]->styleSheet() + "QPushButton{border-bottom: 1px;}");
            d++;
        }
    }
}

QPushButton* MainWindow::genEmptyDayBtn()
{
    QPushButton* empty_btn = new QPushButton(this);
    empty_btn->setFlat(true);
    empty_btn->setMinimumSize(QSize(50, 50));
    empty_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    empty_btn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    empty_btn->setStyleSheet("border: 1px solid rgb(189, 189, 189);"
                             "border-top: 0px;"
                             "border-left: 0px;");

    return empty_btn;
}

QPushButton* MainWindow::genDayBtn(QDate chosen_date)
{
//Container
    QPushButton* day_btn = new QPushButton();
//    QFont font;
//    font.setFamily("Roboto Mono");
//    font.setBold(true);
//    font.setPointSize(9);
//    day_btn->setFont(font);
    day_btn->setFlat(true);
    day_btn->setMinimumSize(QSize(50, 50));
    day_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    day_btn->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    day_btn->setObjectName("day_btn");
    day_btn->setStyleSheet("QPushButton#day_btn {"
                                "border: 1px solid rgb(189, 189, 189);"
                                "border-top: 0px;"
                                "border-left: 0px;"
                                "padding-right: 4px;"
                                "padding-top: 2px;"
                                "background-color: #ffffff;"
                             "}"
                             "QPushButton#day_btn:pressed {"
                             "    background-color: #e0e0e0;"
                             "}"
                             "QPushButton#day_btn:hover:!pressed {"
                             "    background-color: #ffffff;"
                             "}");

//Layout for all elements
    QVBoxLayout* btn_layout = new QVBoxLayout(day_btn);
    btn_layout->setSpacing(0);
    btn_layout->setContentsMargins(6, 6, 6, 6);

//Layout for day of the month label and note button
    QHBoxLayout* top_layout = new QHBoxLayout();
    top_layout->setSpacing(0);
    top_layout->setContentsMargins(0, 0, 0, 0);

    //label
    QCustomLabel* day_lbl = new QCustomLabel(QString::number(chosen_date.day()), day_btn);
    day_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    day_lbl->setFont(QFont("Roboto", 9, 50, false));
    top_layout->addWidget(day_lbl);
    //spacer
    QSpacerItem* top_spc = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Maximum);
    top_layout->addSpacerItem(top_spc);
    //note button or placeholder
    if (!DayController::getNote(chosen_date).isEmpty()) {
        MyIconButton* note_btn = new MyIconButton(QIcon(":/Images/Resources/ic_note_black_24px.svg"), day_btn);
        connect(note_btn, &QPushButton::clicked, [this, chosen_date] {
            current_date.setDate(chosen_date.year(), chosen_date.month(), chosen_date.day());
            is_month_window = true;
            ui->textEdit->setPlainText(DayController::getNote(chosen_date));
            ui->textBrowser->setHtml(DayController::getNote(chosen_date));
            if (ui->switchTextViewBtn->text() == "Edit")
                ui->switchTextViewBtn->click();
            redrawDate();
            ui->stackedWidget->setCurrentIndex(2);
        });
        top_layout->addWidget(note_btn);
    } else {
        MyIconButton* note_placeholder_btn = new MyIconButton(day_btn);
        note_placeholder_btn->setEnabled(false);
        top_layout->addWidget(note_placeholder_btn);
    }
    btn_layout->addLayout(top_layout);

//Layout for "time spent on day tasks" label or placeholder
    if (DayTaskController::dayTaskCount(chosen_date) > 0) {
        QHBoxLayout* mid_layout = new QHBoxLayout;
        mid_layout->setSpacing(0);
        mid_layout->setContentsMargins(0, 0, 0, 0);

        QCustomLabel* task_time_sum_lbl = new QCustomLabel(day_btn);
        QTime duration_sum(0, 0, 0, 0);
        for (int i = 0; i < DayTaskController::dayTaskCount(chosen_date); i++) {
            int h_secs = DayTaskController::getDayTask(chosen_date, i).duration.hour() * 60 * 60;
            int m_secs = DayTaskController::getDayTask(chosen_date, i).duration.minute() * 60;
            duration_sum = duration_sum.addSecs(h_secs + m_secs);
        }
        task_time_sum_lbl->setText(duration_sum.toString("HH:mm"));
        task_time_sum_lbl->setFont(QFont("Roboto", 14, 60, false));
        task_time_sum_lbl->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);

        QSpacerItem* left_spc = new QSpacerItem(10, 10, QSizePolicy::Expanding,QSizePolicy::Maximum);
        QSpacerItem* right_spc = new QSpacerItem(10, 10, QSizePolicy::Expanding,QSizePolicy::Maximum);
        mid_layout->addSpacerItem(left_spc);
        mid_layout->addWidget(task_time_sum_lbl);
        mid_layout->addSpacerItem(right_spc);
        btn_layout->addLayout(mid_layout);
    } else {
        QSpacerItem* mid_spc = new QSpacerItem(10, 10, QSizePolicy::Expanding,QSizePolicy::Expanding);
        btn_layout->addSpacerItem(mid_spc);
    }

//Day status bar or placeholder
    if (chosen_date <= QDate::currentDate()) {
        QPushButton* status = new QPushButton(day_btn);
        status->setMaximumSize(9999999, 5);
        status->setAutoFillBackground(true);
        status->setFlat(true);
        status->setCursor(QCursor(Qt::PointingHandCursor));
        status->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        QString color;
        switch (DayController::getStatus(chosen_date)) {
        case 0:
            color = "background-color: rgb(189, 189, 189);";
            connect(status, &QPushButton::clicked, [this, chosen_date] {
                DayController::setStatus(chosen_date, constants::status_t::bad);
                drawDayButtons();
            });
            break;
        case 1:
            color = "background-color: #e53935;";
            connect(status, &QPushButton::clicked, [this, chosen_date] {
                DayController::setStatus(chosen_date, constants::status_t::ok);
                drawDayButtons();
            });
            break;
        case 2:
            color = "background-color: #ffeb3b;";
            connect(status, &QPushButton::clicked, [this, chosen_date] {
                DayController::setStatus(chosen_date, constants::status_t::good);
                drawDayButtons();
            });
            break;
        case 3:
            color = "background-color: #76ff03;";
            connect(status, &QPushButton::clicked, [this, chosen_date] {
                DayController::setStatus(chosen_date, constants::status_t::none);
                drawDayButtons();
            });
            break;
        }
        status->setStyleSheet(color + "border: 1px; border-radius: 5px;");
        btn_layout->addWidget(status);
    } else {
        QPushButton* status_placeholder = new QPushButton(day_btn);
        status_placeholder->setMaximumSize(9999999, 5);
        status_placeholder->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
        status_placeholder->setFlat(true);
        status_placeholder->setEnabled(false);
        btn_layout->addWidget(status_placeholder);
    }

    connect(day_btn, &QPushButton::clicked, this, [this, chosen_date] () {
        current_date.setDate(chosen_date.year(), chosen_date.month(), chosen_date.day());
        redrawDate();
        drawDayTasks();
        ui->stackedWidget->setCurrentIndex(1);
    });

    return day_btn;
}


void MainWindow::drawDayTasks()
{
//Delete all previous daytask widgets in %daytask_widget_vec
    if (!daytask_widget_vec.empty()) {
        std::for_each(daytask_widget_vec.begin(), daytask_widget_vec.end(), [] (QWidget* widget) { delete widget; });
        daytask_widget_vec.clear();
    }

//Sort daytasks by start time
    std::vector<DayTask> sorted_vec;
    for (int i = 0; i < DayTaskController::dayTaskCount(current_date); i++) {
        sorted_vec.push_back(DayTaskController::getDayTask(current_date, i));
    }
    std::sort(sorted_vec.begin(), sorted_vec.end(), [] (DayTask daytask1, DayTask daytask2) -> bool {
        return daytask1.start_time < daytask2.start_time;
    });

//Fill %daytask_widget_vec with daytask widgets
    for (int i = 0; i < sorted_vec.size(); i++) {
        daytask_widget_vec.push_back(genDayTask(sorted_vec[i]));
    }

//Add daytask widgets to ui
    for (int i = 0; i < daytask_widget_vec.size(); i++) {
        ui->taskLayout->addWidget(daytask_widget_vec[i]);
    }
}

QWidget* MainWindow::genDayTask(const DayTask& daytask)
{
//Assemble daytask widget
//Container
    QWidget* daytask_widget = new QWidget(this);
    //shadow effect
    MyCustomShadowEffect *bodyShadow = new MyCustomShadowEffect();
    bodyShadow->setBlurRadius(6.0);
    bodyShadow->setDistance(2.0);
    bodyShadow->setColor(QColor(0, 0, 0, 20));
    daytask_widget->setGraphicsEffect(bodyShadow);

//Status and icon
    //status
    QFrame* status_color = new QFrame(daytask_widget);
    status_color->setFrameShape(QFrame::VLine);
    status_color->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    status_color->setMinimumWidth(4);
    status_color->setAutoFillBackground(true);
    //QDateTime task_date(current_date, DayTaskController::getEndTime(current_date, daytask.id));
    QString style = "background: %1; border: 0px;";
    switch (daytask.is_done) {
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
    QCustomLabel* icon_lbl = new QCustomLabel(daytask_widget);
    QPixmap icon(task_container.findTask(daytask.task_id).icon_path);
    icon_lbl->setPixmap(icon.scaled(QSize(60,60)));
//Labels
    //task name
    QCustomLabel* text_lbl = new QCustomLabel(task_container.findTask(daytask.task_id).name, daytask_widget);
    text_lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    text_lbl->setFont(QFont("Roboto", 11, 75, false));
    //group name
    QCustomLabel* group_lbl = new QCustomLabel(task_container.getGroup(daytask.task_id).name.toLower(), daytask_widget);
    group_lbl->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    group_lbl->setFont(QFont("Roboto", 10, 75, false));
    group_lbl->setStyleSheet("color: #ffffff;"
                             "border: 1px solid;"
                             "border-color:" + task_container.getGroup(daytask.task_id).color + ";"
                             "background:" + task_container.getGroup(daytask.task_id).color + ";"
                             "border-radius: 4px;");
    //start time
    QCustomLabel* start_time_lbl = new QCustomLabel(daytask.start_time.toString("HH:mm"), daytask_widget);
    start_time_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    start_time_lbl->setFont(QFont("Roboto", 9, 50, false));
    start_time_lbl->setObjectName("start_time_lbl");
    //separator
    QCustomLabel* time_separator_lbl = new QCustomLabel("-", daytask_widget);
    time_separator_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    time_separator_lbl->setFont(QFont("Roboto", 9, 50, false));
    //end time
    QCustomLabel* end_time_lbl = new QCustomLabel(DayTaskController::getEndTime(current_date, daytask.id).toString("HH:mm"), daytask_widget);
    end_time_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    end_time_lbl->setFont(QFont("Roboto", 9, 50, false));
    end_time_lbl->setObjectName("end_time_lbl");
    //separator
    QCustomLabel* duration_separator_lbl = new QCustomLabel(" | ", daytask_widget);
    duration_separator_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    duration_separator_lbl->setFont(QFont("Roboto", 11, 50, false));
    //duration
    QCustomLabel* duration_lbl = new QCustomLabel(daytask_widget);
    duration_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    duration_lbl->setFont(QFont("Roboto", 9, 50, true));
    duration_lbl->setText(QString::number(daytask.duration.hour())
                          + " hour(s) " + QString::number(daytask.duration.minute())
                          + " minute(s)");
//Buttons
    //more
    MyIconButton* more_btn = new MyIconButton(QIcon(":/Images/Resources/ic_more_vert_black_24px.svg"), daytask_widget);
    //context menu and actions
    //!!!!!!!!! %context_menu has to have no parent, otherwise can't call %drawDayTasks() when %remove_task is clicked
    QMenu* context_menu = new QMenu();
    //delete task
    QAction* remove_task = new QAction("Delete", context_menu);
    remove_task->setFont(QFont("Roboto", 9, 50, false));
    connect(remove_task, &QAction::triggered, [this, daytask] {
        if (QMessageBox::warning(this, "Delete confirmation", "Do you really want to delete this task?", QMessageBox::Yes, QMessageBox::Cancel) == QMessageBox::Yes) {
                DayTaskController::removeDayTask(current_date, daytask.id);
                drawDayTasks();
        }
    });
//    QAction* view_breaks = new QAction("View Breaks", context_menu);
//    view_breaks->setFont(QFont("Roboto", 9, 50, false));
//    connect(view_breaks, &QAction::triggered, [this, i] {
//        break_window = new BreakWindow(i, date()[g::y][g::m][g::d].getTask(i).startTime(), date()[g::y][g::m][g::d].getTask(i).endTime());
//        connect(break_window, &BreakWindow::breakAdded, [this] { drawDayTasks(); });
//        break_window->show();
//    });
    //edit task time
    QAction* edit_task_time = new QAction("Edit time", context_menu);
    edit_task_time->setFont(QFont("Roboto", 9, 50, false));
    connect(edit_task_time, &QAction::triggered, [this, daytask] {
        //material dialog template
        MaterialDialog* material_dlg = new MaterialDialog(this);

        //form a layout of controls
        QFormLayout* main_layout = new QFormLayout();

        QLabel* start_time_lbl = new QLabel("Start time", material_dlg);
        QTimeEdit* start_time_edit = new QTimeEdit(QDateTime::currentDateTime().time(), material_dlg);
        start_time_edit->setDisplayFormat("hh:mm");
        start_time_edit->setTime(daytask.start_time);

        main_layout->addRow(start_time_lbl, start_time_edit);

        QLabel* duration_lbl = new QLabel("Duration", material_dlg);
        QTimeEdit* duration_edit = new QTimeEdit(material_dlg);
        duration_edit->setDisplayFormat("hh:mm");
        duration_edit->setTime(daytask.duration);

        main_layout->addRow(duration_lbl, duration_edit);

        //add controls to the dialog
        material_dlg->insertLayout(main_layout);
        material_dlg->setHeading("Edit task time");
        material_dlg->setCancelBtnText("CANCEL");
        material_dlg->setConfirmBtnText("CONFIRM");

        connect(material_dlg, &MaterialDialog::confirmBtnClicked, [this, material_dlg, daytask, start_time_edit, duration_edit] {
            editDayTaskTime(daytask.id, start_time_edit->time(), duration_edit->time());
            material_dlg->close();
        });
        material_dlg->show();
    });
    //mark as done
    QAction* mark_done = new QAction(context_menu);
    mark_done->setFont(QFont("Roboto", 9, 50, false));
    switch (daytask.is_done) {
    case 0:
        mark_done->setText("Mark done");
        connect(mark_done, &QAction::triggered, [this, daytask] {
            DayTaskController::setStatus(current_date, daytask.id, 1);
            drawDayTasks();
        });
        break;
    case 1:
        mark_done->setText("Mark undone");
        connect(mark_done, &QAction::triggered, [this, daytask] {
            DayTaskController::setStatus(current_date, daytask.id, 0);
            drawDayTasks();
        });
        break;
//    case 2:
//        mark_done->setText("Mark done");
//        connect(mark_done, &QAction::triggered, [this, daytask] {
//            int secs = daytask.start_time.secsTo(QTime::currentTime());
//            QTime duration(0,0,0,0);
//            duration.addSecs(secs);
//            DayTaskController::setDuration(current_date, daytask.id, duration);
//            DayTaskController::setStatus(current_date, daytask.id, 1);
//            drawDayTasks();
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
            daytask.start_time <= QTime::currentTime() &&
            DayTaskController::getEndTime(current_date, daytask.id) > QTime::currentTime()) {
        TaskProgressBar* bar = new TaskProgressBar(current_date, daytask, daytask_widget);
        QTimer* timer = new QTimer(daytask_widget);
        connect(timer, &QTimer::timeout, [this, bar]() {
            bar->progress();
        });
        connect(bar, &TaskProgressBar::finished, [this, timer]() {
            delete timer;
            drawDayTasks();
        });
        timer->start(60000);
        bottom_layout->addWidget(bar);
    } else {
        QCustomLabel* break_lbl;
        if (BreakController::breakCount(current_date, daytask.id) > 0) {
            int duration_sum = 0;
            for (size_t j = 0; j < BreakController::breakCount(current_date, daytask.id); j++) {
                QTime start_time = BreakController::getBreak(current_date, daytask.id, j).start_time;
                QTime end_time = BreakController::getBreak(current_date, daytask.id, j).end_time;
                duration_sum += start_time.secsTo(end_time);
            }
            int minute = (duration_sum / 60) % 60;
            duration_sum = (duration_sum / 60) / 60;
            break_lbl = new QCustomLabel(QString::number(BreakController::breakCount(current_date, daytask.id)) + " break(s): " +
                                   QString::number(duration_sum) + " hour(s) and " +
                                   QString::number(minute) + " minute(s) total", daytask_widget);
        } else {
            break_lbl = new QCustomLabel("No breaks", daytask_widget);
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
    daytask_widget->setFixedHeight(84);
    daytask_widget->setLayout(task_layout);
    daytask_widget->setStyleSheet(".QWidget { background-color: #ffffff; }"
                                      ".QCustomLabel { border: 0px; }");

    return daytask_widget;
}

void MainWindow::redrawDate()
{
    //Redraw current date
    ui->monthLabel->setText(QDate::longMonthName(current_date.month()));
    ui->monthLabel_2->setText(QDate::longMonthName(current_date.month()));
    ui->monthLabel_3->setText(QDate::longMonthName(current_date.month()));
    ui->yearLabel->setText(QString::number(current_date.year()));
    ui->yearLabel_2->setText(QString::number(current_date.year()));
    ui->yearLabel_3->setText(QString::number(current_date.year()));
    ui->dayLabel->setText(QString::number(current_date.day()));
    ui->dayLabel_2->setText(QString::number(current_date.day()));
}

void MainWindow::initConnects()
{
//On "change date" button click
    connect(ui->prevMonthBtn, &QPushButton::clicked, this, [this] () {
        if (current_date.month() > 1) {
            current_date.setDate(current_date.year(), current_date.month() - 1, current_date.day());

            if (current_date.year() == constants::START_YEAR && current_date.month() == 1)
                ui->prevMonthBtn->setDisabled(true);

            if (!ui->nextMonthBtn->isEnabled())
                ui->nextMonthBtn->setEnabled(true);

        } else if (current_date.month() == 1) {
            current_date.setDate(current_date.year() - 1, 12, current_date.day());
        }
        redrawDate();
        drawDayButtons();
    });
    connect(ui->nextMonthBtn, &QPushButton::clicked, this, [this] () {
        if (current_date.month() < 12) {
            current_date.setDate(current_date.year(), current_date.month() + 1, current_date.day());

            if (current_date.year() == constants::FINAL_YEAR && current_date.month() == 12)
                ui->nextMonthBtn->setDisabled(true);

            if (!ui->prevMonthBtn->isEnabled())
                ui->prevMonthBtn->setEnabled(true);

        } else if (current_date.month() == 12) {
            current_date.setDate(current_date.year() + 1, 1,current_date.day());
        }
        redrawDate();
        drawDayButtons();
    });

//Note edit settings
    //"switch text view mode" button
    connect(ui->switchTextViewBtn, &QPushButton::clicked, [this] {
        if (ui->switchTextViewBtn->text() == "Edit") {
            ui->switchTextViewBtn->setText("Preview");
            QString text = ui->textEdit->toPlainText();
            text.replace("\n","<br>");
            ui->textBrowser->setText(text);
            ui->stackedWidget_2->setCurrentIndex(1);
        } else {
            ui->switchTextViewBtn->setText("Edit");
            ui->stackedWidget_2->setCurrentIndex(0);
        }
    });
    //Formatting buttons
    //bold
    connect(ui->boldBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<b>" + tmp_cursor.selectedText() + "</b>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //italic
    connect(ui->italicBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<i>" + tmp_cursor.selectedText() + "</i>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //underline
    connect(ui->underBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<u>" + tmp_cursor.selectedText() + "</u>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //strikeout
    connect(ui->strikeBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<s>" + tmp_cursor.selectedText() + "</s>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //subscript
    connect(ui->subBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<sub>" + tmp_cursor.selectedText() + "</sub>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //superscript
    connect(ui->supBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<sup>" + tmp_cursor.selectedText() + "</sup>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //link
    connect(ui->linkBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<a href=\"" + tmp_cursor.selectedText() + "\"></a>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //image
    connect(ui->imgBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        tmp_cursor.insertText("<img src=\"" + tmp_cursor.selectedText() + "\"></img>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //bullet list
    connect(ui->bulletListBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        QString tmp_text = tmp_cursor.selectedText();
        tmp_text.replace(QChar::ParagraphSeparator, "\n");
        tmp_text.replace(QChar::LineSeparator, "\n");
        QStringList text_list = tmp_text.split("\n", QString::SkipEmptyParts);

        tmp_text = "";
        for (QString& entry : text_list) {
            entry.push_front("<li>");
            entry.push_back("</li>\n");
            tmp_text += entry;
        }
        tmp_cursor.insertText("<ul>\n" + tmp_text + "</ul>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });
    //number list
    connect(ui->numListBtn, &QPushButton::clicked, [this] {
        QTextCursor tmp_cursor = ui->textEdit->textCursor();
        QString tmp_text = tmp_cursor.selectedText();
        tmp_text.replace(QChar::ParagraphSeparator, "\n");
        tmp_text.replace(QChar::LineSeparator, "\n");
        QStringList text_list = tmp_text.split("\n", QString::SkipEmptyParts);

        tmp_text = "";
        for (QString& entry : text_list) {
            entry.push_front("<li>");
            entry.push_back("</li>\n");
            tmp_text += entry;
        }
        tmp_cursor.insertText("<ol>\n" + tmp_text + "</ol>");
        ui->textEdit->setTextCursor(tmp_cursor);
    });

//Other connects
    connect(ui->backBtn, &QPushButton::clicked, [this] {
        current_date.setDate(current_date.year(), current_date.month(), 1);
        drawDayButtons();
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->backBtn_2, &QPushButton::clicked, [this] {
        if (DayController::getNote(current_date) != ui->textEdit->toPlainText()) {
            int answer = QMessageBox::warning(this, "Save dialog", "Save changes?", QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
            if (answer == QMessageBox::Yes) {
                DayController::setNote(current_date, ui->textEdit->toPlainText());
            } else if (answer == QMessageBox::Cancel) {
                return;
            }
        }
        if (is_month_window)
            ui->stackedWidget->setCurrentIndex(0);
        else
            ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->backBtn_3, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(0); });
    connect(ui->statBtn, &QPushButton::clicked, [this] { ui->stackedWidget->setCurrentIndex(3); });
    connect(ui->addTaskBtn, &QPushButton::clicked, this, &MainWindow::openTaskDlg);
    connect(ui->saveTextBtn, &QPushButton::clicked, [this]() {
            DayController::setNote(current_date, ui->textEdit->toPlainText());
            ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->noteBtn, &QPushButton::clicked, [this] {
        is_month_window = false;
        ui->textEdit->setPlainText(DayController::getNote(current_date));
        ui->textBrowser->setHtml(DayController::getNote(current_date));
        if (ui->switchTextViewBtn->text() == "Edit")
            ui->switchTextViewBtn->click();
        ui->stackedWidget->setCurrentIndex(2);
    });
}

void MainWindow::editDayTaskTime(int daytask_id, QTime start_time, QTime duration)
{
    DayTaskController::setStartTime(current_date, daytask_id, start_time);
    DayTaskController::setDuration(current_date, daytask_id, duration);

    drawDayTasks();
}

MainWindow::~MainWindow()
{
    delete ui;
}
