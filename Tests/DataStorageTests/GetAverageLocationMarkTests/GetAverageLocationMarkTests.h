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

    DataStorage::GetAverageLocationMarkQuery query_description(1);
    query_description.from_date = std::numeric_limits<long long>::min();
    query_description.to_date = std::numeric_limits<long long>::max();
    query_description.from_age = std::numeric_limits<long long>::min();
    query_description.to_age = std::numeric_limits<long long>::max();

    const auto result =
            data_storage_.GetAverageLocationMark(query_description);
    
    ASSERT_EQ(result, nullptr);
}