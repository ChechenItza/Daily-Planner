#include "models/date.h"
#include "models/task.h"
#include <QDebug>


// DAY {{{
Day::Day(int year, int month, int day)
{
    QDate date(year, month, day);
    weekday = date.dayOfWeek();
    status = constants::status_t::none;
}
// }}} DAY


// MONTH {{{
Month::Month(int month, int year)
{
    QDate date(year, month, 1);

    for (int i = 0; i < date.daysInMonth(); i++)
        day_vec.push_back(Day(year, month, i + 1));
}


Month::Month(const Month& other)
{
    day_vec = other.day_vec;
}


Day& Month::operator[](int index)
{
    return day_vec[index];
}
// }}} MONTH


// YEAR {{{
Year::Year(int y)
{
    for (int m = 0; m < 12; m++)
        month_arr[m] = Month(m + 1, y);
}

Year::Year(const Year& other)
{
    month_arr = other.month_arr;
}


Month& Year::operator[](int index)
{
    return month_arr[index];
}
// }}} YEAR


// DATE {{{
Date::Date(int START_YEAR, int FINAL_YEAR)
{
    for (int y = 0; y < FINAL_YEAR - START_YEAR; y++)
        date_arr[y] = Year(y + START_YEAR);
}

Date::Date(const Date& other)
{
    date_arr = other.date_arr;
}


Date& Date::getInstance()
{
    static Date date(constants::START_YEAR, constants::FINAL_YEAR);
    return date;
}

Year& Date::operator[](int index)
{
    return date_arr[index];
}
// }}} DATE
