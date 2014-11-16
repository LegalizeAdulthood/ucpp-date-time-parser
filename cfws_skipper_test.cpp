// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include <string>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "cfws_skipper.h"

using namespace boost::spirit::qi;

namespace
{

void execute(std::string const& skipped)
{
    const std::string text{skipped + "STOP"};
    auto start = text.begin();

    bool result = phrase_parse(start, text.end(),
        eps, cfws::skipper<std::string::const_iterator>());

    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL("STOP", (std::string{start, text.end()}));
}

}

BOOST_AUTO_TEST_CASE(skips_spaces)
{
    execute("  ");
}

BOOST_AUTO_TEST_CASE(skips_tabs)
{
    execute(" \t \t ");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_at_beginning)
{
    execute("\r\n ");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_in_middle)
{
    execute("    \r\n \t\t");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_at_end)
{
    execute(" \t \t \r\n ");
}
