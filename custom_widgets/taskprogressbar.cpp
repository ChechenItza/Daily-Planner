#include "taskprogressbar.h"
#include <QPainter>
#include <QTime>
#include <QDebug>
#include <QResizeEvent>
#include "settings/constants.h"
#include "controllers/datecontrollers.h"


TaskProgressBar::TaskProgressBar(QDate current_date, DayTask task, QWidget* parent) :
    current_date{current_date},
    task{task},
    QProgressBar(parent)
{
    fragment_count = task.start_time.secsTo(DayTaskController::getEndTime(current_date, task.id));
    if (fragment_count == 0)
        fragment_count = 1;
    else
        fragment_count /= 60;

    i = task.start_time.secsTo(QTime::currentTime()) / 60;

}

void TaskProgressBar::progress()
{
    value += step;
    update();
    if (i < fragment_count - 1)
        i++;
    else
        emit finished();
}

void TaskProgressBar::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    step = this->width() / (float)fragment_count;
    value = step * i;
}

void TaskProgressBar::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPointF start_point;
    start_point.setX(0);
    start_point.setY(13);
    QPointF end_point;
    end_point.setY(13);

    int break_count = 1;

    if (task.break_vec.size() > 0) {
        for (int i = 0; i <= task.break_vec.size() * 2; i++) {
            if (i == task.break_vec.size() * 2) {
                //draw green chunk
                end_point.setX(this->width() - 5);
                painter.setPen(QPen(QColor(constants::red), 2, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(start_point, end_point);
            } else if (i == 0) {
                int task_minutes = task.start_time.secsTo(task.break_vec[0].start_time) / 60;
                float task_chunk = step *  (float)task_minutes;
                //draw red chunk
                end_point.setX(task_chunk);
                painter.setPen(QPen(QColor(constants::red), 2, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(start_point, end_point);

                //set start point at the end of the green chunk
                start_point.setX(end_point.x());
            } else if (i % 2 == 0){
                int task_minutes = task.break_vec[break_count - 2].end_time.secsTo(task.break_vec[break_count - 1].start_time) / 60;
                float task_chunk = step *  (float)task_minutes;
                //draw red chunk
                end_point.setX(end_point.x() + task_chunk);
                painter.setPen(QPen(QColor(constants::red), 2, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(start_point, end_point);

                start_point.setX(end_point.x());
            } else {
                int break_minutes = task.break_vec[i-break_count].start_time.secsTo(task.break_vec[i-break_count].end_time) / 60;
                float break_chunk = step *  (float)break_minutes;
                break_count++;
                //draw green chunk
                end_point.setX(end_point.x() + break_chunk);
                painter.setPen(QPen(QColor(constants::green), 2, Qt::SolidLine, Qt::RoundCap));
                painter.drawLine(start_point, end_point);

                start_point.setX(end_point.x());
            }
        }
    } else {
        end_point.setX(this->width() - 5);
        painter.setPen(QPen(QColor(constants::red), 2, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(start_point, end_point);
    }
    QPointF triangle_start_point;
    triangle_start_point.setX(value - 5.0);
    triangle_start_point.setY(1.0);
    QPointF triangle_bot_point;
    triangle_bot_point.setX(value);
    triangle_bot_point.setY(6.0);
    QPointF triangle_top_point;
    triangle_top_point.setX(value + 5.0);
    triangle_top_point.setY(1.0);
    QPainterPath path;
    path.moveTo(triangle_start_point);
    path.lineTo(triangle_bot_point);
    path.lineTo(triangle_top_point);
    path.lineTo(triangle_start_point);
    painter.setPen(Qt::NoPen);

    painter.fillPath(path, QColor(constants::red));
    for (int i = 0; i < task.break_vec.size(); i++) {
        if (QTime::currentTime() >= task.break_vec[i].start_time &&
                QTime::currentTime() <= task.break_vec[i].end_time)
            painter.fillPath(path, QBrush(QColor(constants::green)));
    }
}

