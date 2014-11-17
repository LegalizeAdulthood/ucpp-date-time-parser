// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/spirit/include/phoenix.hpp>

#include <sstream>
#include <stdexcept>

#include "cfws_skipper.h"
#include "date_time.h"

using namespace boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(::date_time::date,
    (::date_time::days, week_day)
    (unsigned, day)
    (::date_time::months, month)
    (unsigned, year)
);

BOOST_FUSION_ADAPT_STRUCT(::date_time::time,
    (unsigned, hour)
    (unsigned, minute)
    (unsigned, second)
    (int, time_zone_offset)
);

BOOST_FUSION_ADAPT_STRUCT(::date_time::moment,
    (::date_time::date, first)
    (::date_time::time, second)
);

namespace
{

void validate_min_max(char const* name,
    unsigned value, unsigned min_value, unsigned max_value)
{
    if (value < min_value || value > max_value) {
        std::ostringstream message;
        message << name << ' ' << value << " out of range "
            << min_value << '-' << max_value;
        throw std::domain_error(message.str());
    }
}

void validate_day(unsigned day)
{
    validate_min_max("day", day, 1, 31);
}

void validate_year(unsigned year)
{
    validate_min_max("year", year, 1900, 9999);
}

void validate_date(const ::date_time::date& date)
{
    try {
        boost::gregorian::date check(date.year, date.month, date.day);
        if (date.week_day == date_time::Unspecified) {
            return;
        }
        if (static_cast<unsigned>(date.week_day) != check.day_of_week().as_number()) {
            throw std::domain_error("day name doesn't match day of date");
        }
    } catch (const std::out_of_range&) {
        throw std::domain_error("day "
            + boost::lexical_cast<std::string>(date.day)
            + " invalid for month");
    }
}

void validate_hour(unsigned hour)
{
    validate_min_max("hour", hour, 0, 23);
}

void validate_minute(unsigned minute)
{
    validate_min_max("minute", minute, 0, 59);
}

void validate_second(unsigned second)
{
    validate_min_max("second", second, 0, 60);
}

bool last_day_of_June_or_December(::date_time::date const& date)
{
    return (date.month == ::date_time::June && date.day == 30)
        || (date.month == ::date_time::December && date.day == 31);
}

void validate_date_time(::date_time::moment const& moment)
{
    if (moment.second.second == 60
        && !(last_day_of_June_or_December(moment.first)
            && moment.second.hour == 23
            && moment.second.minute == 59)) {
        throw std::domain_error(
            "leap second only allowed on last day of June or December");
    }
}

template <typename Iter>
struct date_time_grammar : grammar<Iter, date_time::moment(), cfws::skipper<Iter>>
{
    typedef cfws::skipper<Iter> skipper;

    date_time_grammar() : date_time_grammar::base_type{start}
    {
        uint_parser<unsigned, 10, 1, 2> digit_1_2;
        uint_parser<unsigned, 10, 2, 2> digit_2;
        uint_parser<unsigned, 10, 3, 3> digit_3;
        uint_parser<unsigned, 10, 4, 4> digit_4;

        month_names.add("Jan", date_time::January)
            ("Feb", date_time::February)
            ("Mar", date_time::March)
            ("Apr", date_time::April)
            ("May", date_time::May)
            ("Jun", date_time::June)
            ("Jul", date_time::July)
            ("Aug", date_time::August)
            ("Sep", date_time::September)
            ("Oct", date_time::October)
            ("Nov", date_time::November)
            ("Dec", date_time::December);
        day_names.add("Mon", date_time::Monday)
            ("Tue", date_time::Tuesday)
            ("Wed", date_time::Wednesday)
            ("Thu", date_time::Thursday)
            ("Fri", date_time::Friday)
            ("Sat", date_time::Saturday)
            ("Sun", date_time::Sunday);
        week_day = (day_names >> ',') | attr(date_time::Unspecified);
        day_number %= digit_1_2[&validate_day];
        year_2 %= digit_2[_val += if_else(_1 < 50U, 2000U, 1900U)];
        year_3 %= digit_3[_val += 1900];
        year_number %= (digit_4 | year_3 | year_2)[&validate_year];
        date_part = week_day >> day_number >> month_names >> year_number;

        seconds = (':' >> digit_2) | attr(0);
        int_parser<int, 10, 4, 4> time_zone_offset;
        time_zone_names.add("UT", +000)("GMT", +000)
            ("EST", -500)("EDT", -400)
            ("CST", -600)("CDT", -500)
            ("MST", -700)("MDT", -600)
            ("PST", -800)("PDT", -700)
            ("A", -100)("B", - 200)("C", - 300)("D", -400)
            ("E", -500)("F", - 600)("G", - 700)("H", -800)
            ("I", -900)("K", -1000)("L", -1100)("M", -1200)
            ("N", +100)("O", + 200)("P", + 300)("Q", +400)
            ("R", +500)("S", + 600)("T", + 700)("U", +800)
            ("V", +900)("W", +1000)("X", +1100)("Y", +1200)
            ("Z", +000);
        time_part %= digit_2[&validate_hour]
            >> lit(':') >> digit_2[&validate_minute]
            >> seconds[&validate_second]
            >> (time_zone_names | time_zone_offset);
        date_time %= date_part[&validate_date] >> time_part;
        start %= date_time[&validate_date_time];
    };

    symbols<char const, date_time::days> day_names;
    rule<Iter, date_time::days()> week_day;
    rule<Iter, unsigned()> day_number;
    symbols<char const, date_time::months> month_names;
    rule<Iter, unsigned()> year_number;
    rule<Iter, unsigned()> year_3;
    rule<Iter, unsigned()> year_2;
    rule<Iter, date_time::date(), skipper> date_part;
    rule<Iter, unsigned(), skipper> seconds;
    symbols<char const, unsigned> time_zone_names;
    rule<Iter, date_time::time(), skipper> time_part;
    rule<Iter, date_time::moment(), skipper> date_time;
    rule<Iter, date_time::moment(), skipper> start;
};

}

namespace date_time
{

moment parse(std::string const& text)
{
    moment result{};
    std::string::const_iterator start{text.begin()};
    if (phrase_parse(start, text.end(),
            date_time_grammar<std::string::const_iterator>{},
            cfws::skipper<std::string::const_iterator>{}, result)
        && start == text.end())
    {
        return result;
    }

    throw std::domain_error("invalid date time");
}

}
