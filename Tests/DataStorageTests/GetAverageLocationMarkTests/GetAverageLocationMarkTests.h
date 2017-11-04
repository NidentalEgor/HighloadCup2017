#include "gtest/gtest.h"

#include "DataStorage.h"

class DataStorageGetAverageLocationMarkTest
    : public TimeMeasurableTest
    , public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    DataStorageGetAverageLocationMarkTest()
        : TestDataLoadableTest(R"(/home/egor/Repositories/highloadcup2017/)"
            R"(Tests/DataStorageTests/GetAverageLocationMarkTests/TestData/)")
    {
    }
};

TEST_F(DataStorageGetAverageLocationMarkTest, NoLocationTest)
{
    LoadData("NoLocationTest/");

    const auto result =
            data_storage_.GetAverageLocationMark(
                DataStorage::GetAverageLocationMarkQuery(1));
    
    ASSERT_EQ(result, nullptr);
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksFoLocationTest)
{
    LoadData("NoMarksFoLocationTest/");

    const auto average_mark =
            data_storage_.GetAverageLocationMark(
                    DataStorage::GetAverageLocationMarkQuery(23));

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":0})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, OneMarkFoLocationTest)
{
    LoadData("OneMarkFoLocationTest/");

    const auto average_mark =
            data_storage_.GetAverageLocationMark(
                    DataStorage::GetAverageLocationMarkQuery(23));

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":3.00000})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeMarksFoLocationTest)
{
    LoadData("ThreeMarksFoLocationTest/");
    
    const auto average_mark =
            data_storage_.GetAverageLocationMark(
                    DataStorage::GetAverageLocationMarkQuery(23));

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":3.66667})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToFromDateTest)
{
    LoadData("NoMarksForLocationDueToFromDateTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.from_date = 1049447314;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":0})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeOfFourMarksForLocationDueToFromDateTest)
{
    LoadData("ThreeOfFourMarksForLocationDueToFromDateTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.from_date = 1049447311;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":3.66667})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToToDateTest)
{
    LoadData("NoMarksForLocationDueToToDateTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.to_date = 1049447314;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":4.66667})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeOfFourMarksForLocationDueToToDateTest)
{
    LoadData("ThreeOfFourMarksForLocationDueToToDateTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.to_date = 1049447314;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":4.33333})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToFromAgeTest)
{
    LoadData("NoMarksForLocationDueToFromAgeTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.from_age = 30;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":0})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeOfFourMarksForLocationDueToFromAgeTest)
{
    LoadData("ThreeOfFourMarksForLocationDueToFromAgeTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.from_age = 10;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":4.33333})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToToAgeTest)
{
    LoadData("NoMarksForLocationDueToToAgeTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.to_age = 1;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":0})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeOfFourMarksForLocationDueToToAgeTest)
{
    LoadData("ThreeOfFourMarksForLocationDueToToAgeTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.to_age = 23;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":3.33333})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToGenderTest)
{
    LoadData("NoMarksForLocationDueToGenderTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.gender = Gender::Female;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":0})");
}

TEST_F(DataStorageGetAverageLocationMarkTest, ThreeOfFourMarksForLocationDueToGenderTest)
{
    LoadData("ThreeOfFourMarksForLocationDueToGenderTest/");
    
    auto query = DataStorage::GetAverageLocationMarkQuery(23);
    query.gender = Gender::Male;

    const auto average_mark =
            data_storage_.GetAverageLocationMark(query);

    ASSERT_NE(average_mark, nullptr);
    ASSERT_EQ(*average_mark, R"({"avg":2.33333})");
}

///
// 1462671912 - 2016 05 08(d) 7 45 12
// 1148172312 - 2006 05 21 7 45 12
// 832898772 - 1996 05 24 7 46 12
// 746153172 - 1993 08 24 7 46 12
///