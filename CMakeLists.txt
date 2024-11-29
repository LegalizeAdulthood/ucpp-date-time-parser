# Copyright (C) 2014, Richard Thomson.  All rights reserved.
cmake_minimum_required(VERSION 2.8.11)
project(date-time-parser CXX)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|Clang)")
    set(CMAKE_CXX_FLAGS "-std=c++11")
endif()
# Locate Boost libraries: unit_test_framework
set(Boost_USE_DYNAMIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.55 REQUIRED COMPONENTS unit_test_framework)

add_executable(date-time-parser-test
    date_time.cpp date_time.h
    date_time_test.cpp
    cfws_skipper.h cfws_skipper_test.cpp
    )
target_include_directories(date-time-parser-test PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(date-time-parser-test ${Boost_LIBRARIES})
add_custom_command(TARGET date-time-parser-test POST_BUILD COMMAND date-time-parser-test)
