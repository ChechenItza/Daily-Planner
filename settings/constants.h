#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>
class QString;

namespace constants
{
    const int START_YEAR = 2016;
    const int FINAL_YEAR = 2099;
    enum status_t { none, bad, ok, good };

    const QString black = "#000000";
    const QString green = "#76ff03";
    const QString dark_green = "#4caf50";
    const QString red = "#E53935";
    const QString purple = "#7e57c2";
    const QString blue = "#3f51b5";

    const std::array<QString, 6> color_arr { black, green, dark_green, red, purple, blue };
}

#endif // CONSTANTS_H
