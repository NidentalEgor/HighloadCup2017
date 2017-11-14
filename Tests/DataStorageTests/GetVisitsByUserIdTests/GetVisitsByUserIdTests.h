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
        : TestDataLoadableTest(R"(/home/egor/Repositories/highloadcup2017/)"
            R"(Tests/DataStorageTests/GetVisitsByUserIdTests/TestData/)")
    {
    }
};

TEST_F(GetVisitsByUserIdTests, NoUserTest)
{
    LoadData("NoUserTest/");

    const auto visit =
            data_storage_.GetVisistsByUserId(
                DataStorage::GetVisistsByUserIdQuery(999));

    ASSERT_EQ(visit, nullptr);
}

TEST_F(GetVisitsByUserIdTests, NoVisitsTest)
{
    LoadData("NoVisitsTest/");

    const auto visit =
            data_storage_.GetVisistsByUserId(
                DataStorage::GetVisistsByUserIdQuery(28));
    
    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToFromDateTest)
{
    LoadData("NoVisitsDueToFromDateTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.from_date = 1049447316;
    const auto visit = data_storage_.GetVisistsByUserId(query); // Boundary case.

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, ThreeOfFourVisitsDueToFromDateTest)
{
    LoadData("ThreeOfFourVisitsDueToFromDateTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.from_date = 1049447316;
    const auto visits = data_storage_.GetVisistsByUserId(query); // Boundary case.

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
    
    DataStorage::GetVisistsByUserIdQuery query(27);
    query.to_date = 1049447314;
    const auto visits = data_storage_.GetVisistsByUserId(query); // Boundary case.

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, ThreeOfFourVisitsDueToToDateTest)
{
    LoadData("ThreeOfFourVisitsDueToToDateTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.to_date = 1049447318;
    const auto visits = data_storage_.GetVisistsByUserId(query); // Boundary case.

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

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.country = "Russia";
    const auto visits = data_storage_.GetVisistsByUserId(query);

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, TwoOfFourVisitsDueToCountryTest)
{
    LoadData("TwoOfFourVisitsDueToCountryTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.country = "Russia";
    const auto visits = data_storage_.GetVisistsByUserId(query);

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447316, "id": 1, "mark": 3},)"
            R"({"user": 27, "location": 23, "visited_at": 1049447317, "id": 2, "mark": 4}]})");
}

TEST_F(GetVisitsByUserIdTests, NoVisitsDueToToDistanceTest)
{
    LoadData("NoVisitsDueToToDistanceTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.to_distance = 100;
    const auto visits = data_storage_.GetVisistsByUserId(query);

    ASSERT_NE(visits, nullptr);
    ASSERT_EQ(*visits, R"({"visits":[]})");
}

TEST_F(GetVisitsByUserIdTests, TwoOfFourVisitsDueToToDistanceTest)
{
    LoadData("TwoOfFourVisitsDueToToDistanceTest/");

    DataStorage::GetVisistsByUserIdQuery query(27);
    query.to_distance = 169;
    const auto visits = data_storage_.GetVisistsByUserId(query);

    ASSERT_NE(visits, nullptr);
    
    AssertEqualJsonDocuments(
            *visits,
            R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447316, "id": 1, "mark": 3},)"
            R"({"user": 27, "location": 23, "visited_at": 1049447317, "id": 2, "mark": 4}]})");
}