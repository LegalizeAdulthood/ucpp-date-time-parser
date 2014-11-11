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
        typedef uint_parser<unsigned, 10, 2, 2> digit_2;
        typedef uint_parser<unsigned, 10, 4, 4> digit_4;
        start = digit_2() >> lit("Jan") >> attr(date_time::January) >> digit_4() >>
                digit_2() >> ':' >> digit_2() >> '+' >> digit_4();
    };

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
