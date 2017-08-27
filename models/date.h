#ifndef DATE_H
#define DATE_H

#include <vector>
#include <array>
#include <QString>
#include "settings/constants.h"
#include "models/daytask.h"


class Day
{
public:
    Day() = default;
    Day(int year, int month, int day);

    int weekday;
    constants::status_t status;
    QString note;
    std::vector<DayTask> daytask_vec;
};

class Month
{
public:
    Month() = default;
    Month(int, int);
    Month(const Month &);

    Day& operator[](int);

    std::vector<Day> day_vec;
};

class Year
{
public:
    Year() = default;
    Year(int);
    Year(const Year &);

    Month& operator[](int);

    std::array<Month, 12> month_arr;
};

class Date
{
public:
    static Date& getInstance();

    Year& operator[](int);

    std::array<Year, constants::FINAL_YEAR - constants::START_YEAR> date_arr;

private:
    Date() = default;
    Date(int START_YEAR, int FINAL_YEAR);
    Date(const Date &);
};
#define date_singleton Date::getInstance()

#endif // DATE_H
