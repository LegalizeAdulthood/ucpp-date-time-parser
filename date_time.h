// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#if !defined(DATE_TIME_H)
#define DATE_TIME_H

#include <string>

namespace date_time
{

enum months
{
    January = 1,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

struct moment
{
    unsigned year;
    months month;
    unsigned day;
    unsigned hour;
    unsigned minute;
    unsigned second;
    int time_zone_offset;
};

moment parse(std::string const& text);

}

#endif
