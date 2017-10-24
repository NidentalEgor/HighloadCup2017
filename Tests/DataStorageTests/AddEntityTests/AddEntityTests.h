#include "gtest/gtest.h"

#include "DataStorage.h"

class AddEntityTest
    : public TimeMeasurableTest
    , public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    AddEntityTest()
        : TestDataLoadableTest(R"(/home/egor/Repositories/HighloadCup2017/)"
            R"(Tests/DataStorageTests/GetAverageLocationMarkTests/TestData/)")
    {
    }
};

TEST_F(AddEntityTest, SimpleAddVisitTest)
{
    data_storage_.AddVisit(
            Visit(1, 2, 3, 769572247, 4));
    
    ASSERT_EQ(data_storage_.GetVisitsAmount(), 1);
}