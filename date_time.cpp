// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include "date_time.h"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <sstream>
#include <stdexcept>

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

typedef ascii::space_type skipper;

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
struct date_time_grammar : grammar<Iter, date_time::moment(), skipper>
{
    date_time_grammar() : date_time_grammar::base_type{start}
    {
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
        uint_parser<unsigned, 10, 1, 2> digit_1_2;
        uint_parser<unsigned, 10, 2, 2> digit_2;
        uint_parser<unsigned, 10, 4, 4> digit_4;
        int_parser<int, 10, 4, 4> time_zone_offset;
        seconds = (':' >> digit_2) | attr(0);
        week_day = (day_names >> ',') | attr(date_time::Unspecified);
        day_number %= digit_1_2[&validate_day];
        year_number %= digit_4[&validate_year];
        date_part = week_day >> day_number >> month_names >> year_number;
        time_part %= digit_2[&validate_hour]
            >> no_skip[lit(':')] >> no_skip[digit_2[&validate_minute]]
            >> no_skip[seconds[&validate_second]] >> time_zone_offset;
        comment = '(' >> *ccontent >> ')';
        ccontent = ctext | quoted_pair | comment;
        ctext = omit[ascii::graph - char_(R"comment_chars(()\)comment_chars")];
        quoted_pair = omit['\\' >> (ascii::graph | ' ' | '\t')];
        date_time %= date_part[&validate_date] >> time_part >> -comment;
        start %= date_time[&validate_date_time];
    };

    symbols<char const, date_time::days> day_names;
    rule<Iter, date_time::days()> week_day;
    rule<Iter, unsigned(), skipper> day_number;
    symbols<char const, date_time::months> month_names;
    rule<Iter, unsigned(), skipper> year_number;
    rule<Iter, date_time::date(), skipper> date_part;
    rule<Iter, unsigned()> seconds;
    rule<Iter, date_time::time(), skipper> time_part;
    rule<Iter, date_time::moment(), skipper> date_time;
    rule<Iter, skipper> comment;
    rule<Iter, skipper> ccontent;
    rule<Iter> ctext;
    rule<Iter> quoted_pair;
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
            ascii::space, result)
        && start == text.end())
    {
        return result;
    }

    throw std::domain_error("invalid date time");
}

}
