// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#include <string>

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "cfws_skipper.h"

using namespace boost::spirit::qi;

namespace
{

bool validate(std::string const& skipped)
{
    const std::string text{skipped + "STOP"};
    auto start = text.begin();

    bool result = phrase_parse(start, text.end(),
        eps, cfws::skipper<std::string::const_iterator>());

    return result
        && "STOP" == std::string{start, text.end()};
}

void execute(std::string const& skipped)
{
    BOOST_REQUIRE(validate(skipped));
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

BOOST_AUTO_TEST_CASE(skips_comments)
{
    execute("(this is a comment)");
}

BOOST_AUTO_TEST_CASE(skips_comments_with_leading_and_trailing_spaces)
{
    execute("( this is a comment )");
}

BOOST_AUTO_TEST_CASE(quoted_whitespace)
{
    execute("(\\ \\\t)");
}

BOOST_AUTO_TEST_CASE(quoted_visible)
{
    execute(R"text((\(\)\\))text");
}

BOOST_AUTO_TEST_CASE(nested_comments)
{
    execute("(this is a comment (with another comment inside))");
}

BOOST_AUTO_TEST_CASE(obsolete_comment_can_contain_control_characters)
{
    BOOST_REQUIRE(validate("(\x01\x02\x03\x04)"));
    BOOST_REQUIRE(validate("(\x05\x06\x07\x08)"));
    BOOST_REQUIRE(validate("(\x0b\x0c)"));
    BOOST_REQUIRE(validate("(\x0e\x0f)"));
    BOOST_REQUIRE(validate("(\x10\x11\x12\x13)"));
    BOOST_REQUIRE(validate("(\x14\x15\x16\x17)"));
    BOOST_REQUIRE(validate("(\x18\x19\x1a\x1b)"));
    BOOST_REQUIRE(validate("(\x1c\x1d\x1e\x1f)"));
}

BOOST_AUTO_TEST_CASE(obsolete_quoted_null)
{
    std::string null{};
    null.push_back(0);
    execute(R"((\)" + null + ")");
}

BOOST_AUTO_TEST_CASE(obsolete_quoted_control)
{
    BOOST_REQUIRE(validate("(\\\x01\\\x02\\\x03\\\x04)"));
    BOOST_REQUIRE(validate("(\\\x05\\\x06\\\x07\\\x08)"));
    BOOST_REQUIRE(validate("(\\\x0b\\\x0c)"));
    BOOST_REQUIRE(validate("(\\\x0e\\\x0f)"));
    BOOST_REQUIRE(validate("(\\\x10\\\x11\\\x12\\\x13)"));
    BOOST_REQUIRE(validate("(\\\x14\\\x15\\\x16\\\x17)"));
    BOOST_REQUIRE(validate("(\\\x18\\\x19\\\x1a\\\x1b)"));
    BOOST_REQUIRE(validate("(\\\x1c\\\x1d\\\x1e\\\x1f)"));
}
