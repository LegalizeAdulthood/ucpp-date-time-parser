// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include "date_time.h"
#define BOOST_DATE_TIME_NO_LIB
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
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
        auto validate_day = [](int day, unused_type, unused_type) {
            if (day < 1 || day > 31) {
                throw std::domain_error("day "
                    + boost::lexical_cast<std::string>(day)
                    + " out of range 1-31.");
            }
        };
        day_number %= digit_1_2[validate_day];
        auto validate_year = [](int year, unused_type, unused_type) {
            if (year < 1900) {
                throw std::domain_error("year "
                    + boost::lexical_cast<std::string>(year)
                    + " before 1900.");
            }
        };
        year_number %= digit_4[validate_year];
        auto validate_date = [](const date_time::date& date, unused_type, unused_type) {
            try {
                boost::gregorian::date check(date.year, date.month, date.day);
                if (date.week_day != date_time::Unspecified
                    && static_cast<unsigned>(date.week_day) != check.day_of_week().as_number()) {
                    throw std::domain_error("day name doesn't match day of date");
                }
            } catch (const std::out_of_range&) {
                throw std::domain_error("day "
                    + boost::lexical_cast<std::string>(date.day)
                    + " invalid for month"); 
            }
        };
        date_part = week_day >> day_number >> month_names >> year_number;
        time_part = digit_2 >> no_skip[lit(':')] >> no_skip[digit_2]
            >> no_skip[seconds] >> time_zone_offset;
        start %= date_part[validate_date] >> time_part;
    };

    symbols<char const, date_time::days> day_names;
    rule<Iter, date_time::days()> week_day;
    rule<Iter, unsigned(), skipper> day_number;
    symbols<char const, date_time::months> month_names;
    rule<Iter, unsigned(), skipper> year_number;
    rule<Iter, date_time::date(), skipper> date_part;
    rule<Iter, unsigned()> seconds;
    rule<Iter, date_time::time(), skipper> time_part;
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
