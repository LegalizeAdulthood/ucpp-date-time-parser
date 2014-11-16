// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#if !defined(FWS_SKIPPER_H)
#define FWS_SKIPPER_H

#include <boost/spirit/include/qi.hpp>

namespace cfws
{

template <typename Iter>
struct skipper : public boost::spirit::qi::grammar<Iter>
{
    skipper() : skipper::base_type{start}
    {
        using namespace boost::spirit::qi;
        wsp = char_(" \t");
        fws = ((*wsp >> lit("\r\n")) | eps) >> wsp;
        ctext = ascii::graph - char_(R"chars(()\)chars");
        quoted_pair = lit('\\') >> (ascii::graph | wsp);
        ccontent = ctext | quoted_pair;
        comment = lit('(') >> *(-fws >> ccontent) >> -fws >> lit(')');
        cfwsp = (+(-fws >> comment) >> -fws)
            | fws;
        start = cfwsp;
    }

    boost::spirit::qi::rule<Iter> wsp;
    boost::spirit::qi::rule<Iter> fws;
    boost::spirit::qi::rule<Iter> ctext;
    boost::spirit::qi::rule<Iter> quoted_pair;
    boost::spirit::qi::rule<Iter> ccontent;
    boost::spirit::qi::rule<Iter> comment;
    boost::spirit::qi::rule<Iter> cfwsp;
    boost::spirit::qi::rule<Iter> start;
};

}

#endif
