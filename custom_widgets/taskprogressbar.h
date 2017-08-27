#ifndef CUSTOMPROGRESSBAR_H
#define CUSTOMPROGRESSBAR_H
#include <QProgressBar>
#include "models/daytask.h"

class TaskProgressBar : public QProgressBar
{
Q_OBJECT
public:
    TaskProgressBar(QDate current_date, DayTask task, QWidget* parent = 0);

public slots:
    void progress();
signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *) Q_DECL_OVERRIDE;

private:
    void setStep();

    std::vector<QTime> break_vec;
    QDate current_date;
    DayTask task;
    float value;
    int fragment_count;
    float step;
    int i;
};

#endif // CUSTOMPROGRESSBAR_H
