#ifndef JSON_COMPARABLE_TEST_H_INCLUDED
#define JSON_COMPARABLE_TEST_H_INCLUDED
#pragma once

#include <string>

#include "gtest/gtest.h"
#include "rapidjson/document.h"

class JsonComparableTest
{
public:
    virtual ~JsonComparableTest() = default;

protected:
    void AssertEqualJsonDocuments(
            const std::string& actual_json_content,
            const std::string& expected_json_content) const
    {
        // Sorted by date.
        rapidjson::Document actual;
        actual.Parse(actual_json_content.c_str());

        rapidjson::Document expected;
        expected.Parse(expected_json_content.c_str());

        ASSERT_EQ(actual, expected);
    }
};

#endif // JSON_COMPARABLE_TEST_H_INCLUDED