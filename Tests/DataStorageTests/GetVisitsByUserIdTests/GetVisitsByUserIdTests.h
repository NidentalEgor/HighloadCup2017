#include "gtest/gtest.h"
#include "rapidjson/document.h"

#include "DataStorage.h"

class GetVisitsByUserIdTests
    : public TimeMeasurableTest
    , public testing::Test
{
protected:
    void LoadData(const std::string& test_data_folder_name)
    {
        data_storage_.LoadData(test_data_folder_path_ + test_data_folder_name);
    }

protected:
    DataStorage data_storage_;
    std::string test_data_folder_path_ = R"(/home/egor/Repositories/HighloadCup2017/Tests/DataStorageTests/GetVisitsByUserIdTests/TestData/)";
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

TEST_F(GetVisitsByUserIdTests, TwoOfThreeVisitsDueToFromDateTest)
{
    LoadData("TwoOfThreeVisitsDueToFromDateTest/");

    const auto visits = data_storage_.GetVisistsByUserId(27, 1049447316); // Boundary case.

    ASSERT_NE(visits, nullptr);
    
    rapidjson::Document actual;
    actual.Parse(visits->c_str());

    rapidjson::Document expected;
    // Sorted by date.
    expected.Parse(R"({"visits": [{"user": 27, "location": 22, "visited_at": 1049447317, "id": 4, "mark": 4},)"
                    R"({"user": 27, "location": 22, "visited_at": 1049447317, "id": 2, "mark": 4},)"
                    R"({"user": 27, "location": 22, "visited_at": 1049447318, "id": 3, "mark": 4}]})");
    
    ASSERT_EQ(actual, expected);
}
