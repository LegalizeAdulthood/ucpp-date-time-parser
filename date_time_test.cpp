// Copyright (C) 2014, Richard Thomson.  All rights reserved.
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <boost/test/unit_test.hpp>
#include "date_time.h"

BOOST_AUTO_TEST_CASE(january_1st_2010_noon_utc)
{
    const std::string text{"01 Jan 2010 12:00 +0000"};

    const auto value = date_time::parse(text);

    BOOST_REQUIRE_EQUAL(2010, value.year);
    BOOST_REQUIRE_EQUAL(date_time::January, value.month);
    BOOST_REQUIRE_EQUAL(1, value.day);
    BOOST_REQUIRE_EQUAL(12, value.hour);
    BOOST_REQUIRE_EQUAL(0, value.minute);
    BOOST_REQUIRE_EQUAL(0, value.second);
    BOOST_REQUIRE_EQUAL(0, value.time_zone_offset);
}
