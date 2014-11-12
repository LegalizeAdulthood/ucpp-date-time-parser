// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#if !defined(DATE_TIME_H)
#define DATE_TIME_H

#include <string>
#include <utility>

namespace date_time
{

enum days
{
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
    Sunday,
    Unspecified
};

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

struct date
{
    days week_day;
    unsigned year;
    months month;
    unsigned day;
};

struct time
{
    unsigned hour;
    unsigned minute;
    unsigned second;
    int time_zone_offset;
};

typedef std::pair<date, time> moment;

moment parse(std::string const& text);

}

#endif
