#include "gtest/gtest.h"

#include "DataStorage.h"
#include "../../TimeMeasurableTest.h"
#include "../../TestDataLoadableTest.h"
#include "../../JsonComparableTest.h"

class GetVisitsByUserIdTests
    : public TimeMeasurableTest
    , public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    GetVisitsByUserIdTests()
        : TestDataLoadableTest(R"(/home/egor/Repositories/HighloadCup2017/)"
            R"(Tests/DataStorageTests/GetVisitsByUserIdTests/TestData/)")
    {
    }
};

TEST_F(GetVisitsByUserIdTests, NoUserTest)
{
    LoadData("NoUserTest/");

    const auto visit = data_storage_.GetVisistsByUserId(999);

    ASSERT_EQ(visit, nullptr);
}

TEST_F(GetVisitsByUserIdTests, NoVisitsTest)
{
    LoadData("NoVisitsTest/");

    const auto visit = data_storage_.GetVisistsByUserId(28);

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToFromDateTest)
{
    LoadData("NoVisitsDueToFromDateTest/");

    const auto visit = data_storage_.GetVisistsByUserId(27, 1049447316); // Boundary case.

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, ThreeOfFourVisitsDueToFromDateTest)
{
    LoadData("ThreeOfFourVisitsDueToFromDateTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447316); // Boundary case.

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447317, "id": 4, "mark": 4},)"
            R"({"user": 27, "location": 22, "visited_at": 1049447317, "id": 2, "mark": 4},)"
            R"({"user": 27, "location": 22, "visited_at": 1049447318, "id": 3, "mark": 4}]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToToDateTest)
{
    LoadData("NoVisitsDueToToDateTest/");
    
    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447314); // Boundary case.

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, ThreeOfFourVisitsDueToToDateTest)
{
    LoadData("ThreeOfFourVisitsDueToToDateTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447318); // Boundary case.

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447316, "id": 1, "mark": 3},)"
            R"({"user": 27, "location": 22, "visited_at": 1049447317, "id": 4, "mark": 4},)"
            R"({"user": 27, "location": 22, "visited_at": 1049447317, "id": 2, "mark": 4}]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToCountryTest)
{
    LoadData("NoVisitsDueToCountryTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447319, "Russia");

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, TwoOfFourVisitsDueToCountryTest)
{
    LoadData("TwoOfFourVisitsDueToCountryTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447319, "Russia");

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447316, "id": 1, "mark": 3},)"
            R"({"user": 27, "location": 23, "visited_at": 1049447317, "id": 2, "mark": 4}]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToToDistanceTest)
{
    LoadData("NoVisitsDueToToDistanceTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447319, "Russia", 100);

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, TwoOfFourVisitsDueToToDistanceTest)
{
    LoadData("TwoOfFourVisitsDueToToDistanceTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447100, 1049447319, "Russia", 169);

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447316, "id": 1, "mark": 3},)"
            R"({"user": 27, "location": 23, "visited_at": 1049447317, "id": 2, "mark": 4}]})");
}