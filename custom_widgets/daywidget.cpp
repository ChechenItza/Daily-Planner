#include "daywidget.h"
#include "custom_widgets/qcustomlabel.h"
#include "custom_widgets/myiconbutton.h"
#include "controllers/datecontrollers.h"
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

DayWidget::DayWidget(QDate date, QWidget* parent) :
    QPushButton(parent),
    chosen_date(date)
{
    //    QFont font;
    //    font.setFamily("Roboto Mono");
    //    font.setBold(true);
    //    font.setPointSize(9);
    //    this->setFont(font);
        this->setFlat(true);
        this->setMinimumSize(QSize(50, 50));
        this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        this->setObjectName("this");
        this->setStyleSheet("QPushButton#this {"
                                    "border: 1px solid rgb(189, 189, 189);"
                                    "border-top: 0px;"
                                    "border-left: 0px;"
                                    "padding-right: 4px;"
                                    "padding-top: 2px;"
                                    "background-color: #ffffff;"
                                 "}"
                                 "QPushButton#this:pressed {"
                                 "    background-color: #e0e0e0;"
                                 "}"
                                 "QPushButton#this:hover:!pressed {"
                                 "    background-color: #ffffff;"
                                 "}");

    //Layout for all elements
        QVBoxLayout* btn_layout = new QVBoxLayout(this);
        btn_layout->setSpacing(0);
        btn_layout->setContentsMargins(6, 6, 6, 6);

    //Layout for day of the month label and note button
        QHBoxLayout* top_layout = new QHBoxLayout();
        top_layout->setSpacing(0);
        top_layout->setContentsMargins(0, 0, 0, 0);

        //label
        QCustomLabel* day_lbl = new QCustomLabel(QString::number(chosen_date.day()), this);
        day_lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        day_lbl->setFont(QFont("Roboto", 9, 50, false));
        top_layout->addWidget(day_lbl);
        //spacer
        QSpacerItem* top_spc = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Maximum);
        top_layout->addSpacerItem(top_spc);
        //note button or placeholder
        if (!DayController::getNote(chosen_date).isEmpty()) {
            MyIconButton* note_btn = new MyIconButton(QIcon(":/Images/Resources/ic_note_black_24px.svg"), this);
            connect(note_btn, &QPushButton::clicked, [this] {
                emit noteClicked();
            });
            top_layout->addWidget(note_btn);
        } else {
            MyIconButton* note_placeholder_btn = new MyIconButton(this);
            note_placeholder_btn->setEnabled(false);
            top_layout->addWidget(note_placeholder_btn);
        }
        btn_layout->addLayout(top_layout);

    //Layout for "time spent on day tasks" label or placeholder
        if (TaskController::TaskCount(chosen_date) > 0) {
            QHBoxLayout* mid_layout = new QHBoxLayout;
            mid_layout->setSpacing(0);
            mid_layout->setContentsMargins(0, 0, 0, 0);

            QCustomLabel* task_time_sum_lbl = new QCustomLabel(this);
            QTime duration_sum(0, 0, 0, 0);
            for (int i = 0; i < TaskController::TaskCount(chosen_date); i++) {
                int h_secs = TaskController::getTask(chosen_date, i).duration.hour() * 60 * 60;
                int m_secs = TaskController::getTask(chosen_date, i).duration.minute() * 60;
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
            QPushButton* status = new QPushButton(this);
            status->setMaximumSize(9999999, 5);
            status->setAutoFillBackground(true);
            status->setFlat(true);
            status->setCursor(QCursor(Qt::PointingHandCursor));
            status->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            QString color;
            switch (DayController::getStatus(chosen_date)) {
            case 0:
                color = "background-color: rgb(189, 189, 189);";
                connect(status, &QPushButton::clicked, [this] {
                    DayController::setStatus(chosen_date, constants::status_t::bad);
                    emit redrawIsNeeded();
                });
                break;
            case 1:
                color = "background-color: #e53935;";
                connect(status, &QPushButton::clicked, [this] {
                    DayController::setStatus(chosen_date, constants::status_t::ok);
                    emit redrawIsNeeded();
                });
                break;
            case 2:
                color = "background-color: #ffeb3b;";
                connect(status, &QPushButton::clicked, [this] {
                    DayController::setStatus(chosen_date, constants::status_t::good);
                    emit redrawIsNeeded();
                });
                break;
            case 3:
                color = "background-color: #76ff03;";
                connect(status, &QPushButton::clicked, [this] {
                    DayController::setStatus(chosen_date, constants::status_t::none);
                    emit redrawIsNeeded();
                });
                break;
            }
            status->setStyleSheet(color + "border: 1px; border-radius: 5px;");
            btn_layout->addWidget(status);
        } else {
            QPushButton* status_placeholder = new QPushButton(this);
            status_placeholder->setMaximumSize(9999999, 5);
            status_placeholder->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
            status_placeholder->setFlat(true);
            status_placeholder->setEnabled(false);
            btn_layout->addWidget(status_placeholder);
        }
}

QDate DayWidget::getDate()
{
    return chosen_date;
}
