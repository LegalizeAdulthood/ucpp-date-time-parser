// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#if !defined(FWS_SKIPPER_H)
#define FWS_SKIPPER_H

#include <boost/spirit/include/qi.hpp>

namespace cfws
{

template <typename Iter>
struct skipper : boost::spirit::qi::grammar<Iter>
{
    skipper() : skipper::base_type{start}
    {
        wsp = char_(" \t");
        start = -(*wsp >> lit("\r\n")) >> wsp;
    }

    boost::spirit::qi::rule<Iter> start;
    boost::spirit::qi::rule<Iter> wsp;
};

}

#endif
