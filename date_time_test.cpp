// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "date_time.h"

BOOST_AUTO_TEST_CASE(january_1st_2010_noon_utc)
{
    const std::string text{"01 Jan 2010 12:00 +0000"};

    const auto value = date_time::parse(text);

    BOOST_REQUIRE_EQUAL(date_time::Unspecified, value.first.week_day);
    BOOST_REQUIRE_EQUAL(2010, value.first.year);
    BOOST_REQUIRE_EQUAL(date_time::January, value.first.month);
    BOOST_REQUIRE_EQUAL(1, value.first.day);
    BOOST_REQUIRE_EQUAL(12, value.second.hour);
    BOOST_REQUIRE_EQUAL(0, value.second.minute);
    BOOST_REQUIRE_EQUAL(0, value.second.second);
    BOOST_REQUIRE_EQUAL(0, value.second.time_zone_offset);
}

BOOST_AUTO_TEST_CASE(february)
{
    const std::string text{"01 Feb 2010 12:00 +0000"};

    const auto value = date_time::parse(text).first;

    BOOST_REQUIRE_EQUAL(date_time::February, value.month);
}

BOOST_AUTO_TEST_CASE(single_digit_day)
{
    const std::string text{"9 Jan 2010 12:00 +0000"};

    const auto value = date_time::parse(text).first;

    BOOST_REQUIRE_EQUAL(9, value.day);
}

BOOST_AUTO_TEST_CASE(time_zone_offset)
{
    const std::string text{"9 Jan 2010 12:00 +0400"};

    const auto value = date_time::parse(text).second;

    BOOST_REQUIRE_EQUAL(400, value.time_zone_offset);
}

BOOST_AUTO_TEST_CASE(negative_time_zone_offset)
{
    const std::string text{"9 Jan 2010 12:00 -0400"};

    const auto value = date_time::parse(text).second;

    BOOST_REQUIRE_EQUAL(-400, value.time_zone_offset);
}

BOOST_AUTO_TEST_CASE(time_with_seconds)
{
    const std::string text{"9 Jan 2010 12:00:45 -0400"};

    const auto value = date_time::parse(text).second;

    BOOST_REQUIRE_EQUAL(45, value.second);
}

BOOST_AUTO_TEST_CASE(weekday)
{
    const std::string text{"Sat, 9 Jan 2010 12:00:45 -0400"};

    const auto value = date_time::parse(text).first;

    BOOST_REQUIRE_EQUAL(date_time::Saturday, value.week_day);
}

BOOST_AUTO_TEST_CASE(no_space_allowed_in_time)
{
    BOOST_REQUIRE_THROW(date_time::parse("9 Jan 2010 12 :00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("9 Jan 2010 12: 00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("9 Jan 2010 12:00 :45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("9 Jan 2010 12:00: 45 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(no_space_allowed_in_weekday)
{
    BOOST_REQUIRE_THROW(date_time::parse("Sat , 9 Jan 2010 12:00:45 -0400"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(day_numbers_outside_1_to_31_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("0 Jan 2010 12:00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("32 Jan 2010 12:00:45 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(years_before_1900_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("1 Jan 1899 12:00:45 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(day_numbers_outside_month_range_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("29 Feb 2010 12:00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Apr 2010 12:00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Jun 2010 12:00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Sep 2010 12:00:45 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Nov 2010 12:00:45 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(leap_days_are_valid)
{
    BOOST_REQUIRE_NO_THROW(date_time::parse("29 Feb 2008 12:00:45 +0000"));
}

BOOST_AUTO_TEST_CASE(day_name_not_matching_date_is_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("Tue, 1 Feb 2008 12:00:45 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(hours_outside_0_to_23_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("1 Feb 2008 24:00:00 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(minutes_outside_0_to_59_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("1 Feb 2008 23:60:00 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(seconds_outside_0_to_60_are_invalid)
{
    BOOST_REQUIRE_THROW(date_time::parse("1 Feb 2008 23:59:61 +0000"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(leap_second_only_added_on_last_day_of_June_or_December)
{
    BOOST_REQUIRE_THROW(date_time::parse("1 Feb 2008 23:59:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("29 Jun 2008 23:59:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("30 Jun 2008 00:00:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("30 Jun 2008 00:59:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("30 Jun 2008 23:00:60 +0000"), std::domain_error);
    BOOST_REQUIRE_NO_THROW(date_time::parse("30 Jun 2008 23:59:60 +0000"));
    BOOST_REQUIRE_THROW(date_time::parse("30 Dec 2008 23:59:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Dec 2008 00:00:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Dec 2008 00:59:60 +0000"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("31 Dec 2008 23:00:60 +0000"), std::domain_error);
    BOOST_REQUIRE_NO_THROW(date_time::parse("31 Dec 2008 23:59:60 +0000"));
}

BOOST_AUTO_TEST_CASE(commented_date)
{
    BOOST_REQUIRE_NO_THROW(date_time::parse("Sat, 9 Jan 2010 12:00:45 -0400 (Starting Date)"));
}

BOOST_AUTO_TEST_CASE(comment_with_quoted_pair)
{
    BOOST_REQUIRE_NO_THROW(date_time::parse(R"date(Sat, 9 Jan 2010 12:00:45 -0400 (Starting\ Date))date"));
}

BOOST_AUTO_TEST_CASE(invalid_quoted_pair_in_comment)
{
    BOOST_REQUIRE_THROW(date_time::parse("Sat, 9 Jan 2010 12:00:45 -0400 (\\\r)"), std::domain_error);
}

BOOST_AUTO_TEST_CASE(comment_inside_comment)
{
    BOOST_REQUIRE_NO_THROW(date_time::parse(R"date(Sat, 9 Jan 2010 12:00:45 -0400 (Comment (with another inside)))date"));
}

BOOST_AUTO_TEST_CASE(comment_delimiters_must_be_matched_or_quoted)
{
    BOOST_REQUIRE_THROW(date_time::parse("Sat, 9 Jan 2010 12:00:45 -0400 (Starting (Date)"), std::domain_error);
    BOOST_REQUIRE_THROW(date_time::parse("Sat, 9 Jan 2010 12:00:45 -0400 (Starting Date))"), std::domain_error);
    BOOST_REQUIRE_NO_THROW(date_time::parse(R"date(Sat, 9 Jan 2010 12:00:45 -0400 (Starting \(Date))date"));
    BOOST_REQUIRE_NO_THROW(date_time::parse(R"date(Sat, 9 Jan 2010 12:00:45 -0400 (Starting Date\)))date"));
}
