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
        : TestDataLoadableTest(R"(/home/egor/Repositories/HighloadCup2017/)"
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

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToFromDate)
{
    LoadData("NoMarksForLocationDueToFromDate/");
    
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

TEST_F(DataStorageGetAverageLocationMarkTest, NoMarksForLocationDueToToDate)
{
    LoadData("NoMarksForLocationDueToToDate/");
    
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