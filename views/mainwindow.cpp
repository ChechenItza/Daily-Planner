#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QDate"
#include "settings/constants.h"
#include "controllers/db.h"
#include "controllers/datecontrollers.h"
#include "custom_widgets/mycustomshadoweffect.h"
#include "views/taskdialog.h"
#include <QMessageBox>
#include <QStyle>
#include <QDesktopWidget>
#include "custom_widgets/daytaskwidget.h"
#include "custom_widgets/daywidget.h"
#include "custom_widgets/emptydaywidget.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    current_date(QDate::currentDate())
{
    ui->setupUi(this);
    db;

    ui->monthBox->addItem("January");
    ui->monthBox->addItem("February");
    ui->monthBox->addItem("March");
    ui->monthBox->addItem("April");
    ui->monthBox->addItem("May");
    ui->monthBox->addItem("June");
    ui->monthBox->addItem("July");
    ui->monthBox->addItem("August");
    ui->monthBox->addItem("September");
    ui->monthBox->addItem("October");
    ui->monthBox->addItem("November");
    ui->monthBox->addItem("December");

    for (int i = constants::START_YEAR; i < constants::FINAL_YEAR; i++) {
        ui->yearBox->addItem(QString::number(i));
    }

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
        EmptyDayWidget* empty_day_widget = new EmptyDayWidget(this);
        daybutton_vec.push_back(empty_day_widget);
    }

    //fill it with normal buttons until the last weekday of the month
    QDate tmp_date;
    for (int i = month_date.dayOfWeek(); i < month_date.daysInMonth() + month_date.dayOfWeek(); i++) {
        tmp_date.setDate(month_date.year(), month_date.month(), i + 1 - month_date.dayOfWeek());
        DayWidget* day_widget = new DayWidget(tmp_date, this);
        connect(day_widget, &QPushButton::clicked, [this, day_widget] {
            QDate tmp_date(day_widget->getDate());
            current_date.setDate(tmp_date.year(), tmp_date.month(), tmp_date.day());
            redrawDate();
            drawDayTasks();
            ui->stackedWidget->setCurrentIndex(1);
        });
        connect(day_widget, &DayWidget::redrawIsNeeded, [this] {
            drawDayButtons();
        });
        connect(day_widget, &DayWidget::noteClicked, [this, day_widget] {
            QDate tmp_date(day_widget->getDate());
            current_date.setDate(tmp_date.year(), tmp_date.month(), tmp_date.day());
            is_month_window = true;
            ui->textEdit->setPlainText(DayController::getNote(current_date));
            ui->textBrowser->setHtml(DayController::getNote(current_date));
            if (ui->switchTextViewBtn->text() == "Edit")
                ui->switchTextViewBtn->click();
            redrawDate();
            ui->stackedWidget->setCurrentIndex(2);
        });
        daybutton_vec.push_back(day_widget);
    }

    //fill the rest with empty buttons
    for (int i = month_date.daysInMonth() + month_date.dayOfWeek(); i < 42 + 1; i++) {
        EmptyDayWidget* empty_day_widget = new EmptyDayWidget(this);
        daybutton_vec.push_back(empty_day_widget);
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
        DayTaskWidget* daytask_widget = new DayTaskWidget(current_date, sorted_vec[i], this);
        daytask_widget_vec.push_back(daytask_widget);
    }

//Add daytask widgets to ui
    for (int i = 0; i < daytask_widget_vec.size(); i++) {
        ui->taskLayout->addWidget(daytask_widget_vec[i]);
    }
}

void MainWindow::redrawDate()
{
    //Redraw current date
    ui->monthBox->setCurrentIndex(current_date.month() - 1);
    ui->monthLabel_2->setText(QDate::longMonthName(current_date.month()));
    ui->monthLabel_3->setText(QDate::longMonthName(current_date.month()));
    ui->yearBox->setCurrentIndex(current_date.year() - constants::START_YEAR);
    ui->yearLabel_2->setText(QString::number(current_date.year()));
    ui->yearLabel_3->setText(QString::number(current_date.year()));
    ui->dayLabel->setText(QString::number(current_date.day()));
    ui->dayLabel_2->setText(QString::number(current_date.day()));
}

void MainWindow::initConnects()
{
//On "change date" button click
    connect(ui->monthBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] (int index) {
        current_date.setDate(current_date.year(), index + 1, current_date.day());
        drawDayButtons();
    });
    connect(ui->yearBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), [this] (int index) {
        current_date.setDate(index + constants::START_YEAR, current_date.month(), current_date.day());
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

MainWindow::~MainWindow()
{
    delete ui;
}
