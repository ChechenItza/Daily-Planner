#ifndef BREAK_H
#define BREAK_H

class QTime;

struct Break
{
    QTime start_time;
    QTime end_time;
    int id;
};

#endif // BREAK_H
