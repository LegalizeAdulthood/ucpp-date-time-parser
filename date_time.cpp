// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include "date_time.h"
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <stdexcept>

using namespace boost::spirit::qi;

BOOST_FUSION_ADAPT_STRUCT(date_time::moment,
    (unsigned, day)
    (date_time::months, month)
    (unsigned, year)
    (unsigned, hour)
    (unsigned, minute)
    (unsigned, second)
    (int, time_zone_offset)
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
        typedef uint_parser<unsigned, 10, 2, 2> digit_2;
        typedef uint_parser<unsigned, 10, 4, 4> digit_4;
        start = digit_2() >> month_names >> digit_4() >>
                digit_2() >> ':' >> digit_2() >> '+' >> digit_4();
    };

    symbols<char const, date_time::months> month_names;
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
            date_time_grammar<std::string::const_iterator>(),
            ascii::space, result)
        && start == text.end())
    {
        return result;
    }

    throw std::domain_error("invalid date time");
}

}
