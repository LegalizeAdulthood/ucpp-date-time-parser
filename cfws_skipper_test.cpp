// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include <string>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "cfws_skipper.h"

using namespace boost::spirit::qi;

namespace
{

struct fixture
{
    fixture()
        : suffix("STOP")
    {}
    bool execute(std::string const& skipped);

    const std::string suffix;
    std::string text;
    std::string::iterator start;
};

bool fixture::execute(std::string const& skipped)
{
    text = skipped + suffix;
    start = text.begin();

    return phrase_parse(start, text.end(), eps,
        cfws::skipper<decltype(start)>());
}

}

BOOST_FIXTURE_TEST_SUITE(cfws_skipper_suite, ::fixture);

#define REQUIRE_SKIPPED(text_) \
    BOOST_REQUIRE(execute(text_)); \
    BOOST_REQUIRE_EQUAL(suffix, (std::string{start, text.end()}))

BOOST_AUTO_TEST_CASE(skips_spaces)
{
    REQUIRE_SKIPPED("  ");
}

BOOST_AUTO_TEST_CASE(skips_tabs)
{
    REQUIRE_SKIPPED(" \t \t ");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_at_beginning)
{
    REQUIRE_SKIPPED("\r\n ");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_in_middle)
{
    REQUIRE_SKIPPED("    \r\n \t\t");
}

BOOST_AUTO_TEST_CASE(skips_folded_whitespace_at_end)
{
    REQUIRE_SKIPPED(" \t \t \r\n ");
}

BOOST_AUTO_TEST_CASE(skips_comments)
{
    REQUIRE_SKIPPED("(this is a comment)");
}

BOOST_AUTO_TEST_CASE(skips_comments_with_leading_and_trailing_spaces)
{
    REQUIRE_SKIPPED("( this is a comment )");
}

BOOST_AUTO_TEST_CASE(quoted_whitespace)
{
    REQUIRE_SKIPPED("(\\ \\\t)");
}

BOOST_AUTO_TEST_CASE(quoted_visible)
{
    REQUIRE_SKIPPED(R"text((\(\)\\))text");
}

BOOST_AUTO_TEST_CASE(nested_comments)
{
    REQUIRE_SKIPPED("(this is a comment (with another comment inside))");
}

BOOST_AUTO_TEST_CASE(obsolete_comment_can_contain_control_characters)
{
    REQUIRE_SKIPPED("(\x01\x02\x03\x04)");
    REQUIRE_SKIPPED("(\x05\x06\x07\x08)");
    REQUIRE_SKIPPED("(\x0b\x0c)");
    REQUIRE_SKIPPED("(\x0e\x0f)");
    REQUIRE_SKIPPED("(\x10\x11\x12\x13)");
    REQUIRE_SKIPPED("(\x14\x15\x16\x17)");
    REQUIRE_SKIPPED("(\x18\x19\x1a\x1b)");
    REQUIRE_SKIPPED("(\x1c\x1d\x1e\x1f)");
}

BOOST_AUTO_TEST_CASE(obsolete_quoted_null)
{
    std::string null{};
    null.push_back(0);
    REQUIRE_SKIPPED(R"((\)" + null + ")");
}

BOOST_AUTO_TEST_CASE(obsolete_quoted_control)
{
    REQUIRE_SKIPPED("(\\\x01\\\x02\\\x03\\\x04)");
    REQUIRE_SKIPPED("(\\\x05\\\x06\\\x07\\\x08)");
    REQUIRE_SKIPPED("(\\\x0b\\\x0c)");
    REQUIRE_SKIPPED("(\\\x0e\\\x0f)");
    REQUIRE_SKIPPED("(\\\x10\\\x11\\\x12\\\x13)");
    REQUIRE_SKIPPED("(\\\x14\\\x15\\\x16\\\x17)");
    REQUIRE_SKIPPED("(\\\x18\\\x19\\\x1a\\\x1b)");
    REQUIRE_SKIPPED("(\\\x1c\\\x1d\\\x1e\\\x1f)");
}

BOOST_AUTO_TEST_SUITE_END();
