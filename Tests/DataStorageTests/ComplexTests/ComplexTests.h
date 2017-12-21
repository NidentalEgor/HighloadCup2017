#include "gtest/gtest.h"

#include "../../../DataStorage/DataStorage.h"

class DataStorageComplexTest
    : public TimeMeasurableTest
    , public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    DataStorageComplexTest()
        : TestDataLoadableTest(R"(/home/egor/Repositories/highloadcup2017/)"
        R"(Tests/DataStorageTests/ComplexTests/TestData/)")
    {
    }

    void CheckVisits(
            const char* visits_of_first_user,
            const char* visits_of_second_user)
    {
        {
            const auto visits =
                    data_storage_.GetVisistsByUserId(
                        DataStorage::GetVisistsByUserIdQuery(1));

            ASSERT_NE(visits, nullptr);

            AssertEqualJsonDocuments(
                    *visits,
                    visits_of_first_user);
        }
        
        {
            const auto visits =
                    data_storage_.GetVisistsByUserId(
                        DataStorage::GetVisistsByUserIdQuery(2));

            ASSERT_NE(visits, nullptr);
            AssertEqualJsonDocuments(
                    *visits,
                    visits_of_second_user);
        }
    }

    void CheckAverageMark(
            const char* average_mark_of_first_location,
            const char* average_mark_of_second_location)
    {
        {
            const auto average_mark =
                    data_storage_.GetAverageLocationMark(
                        DataStorage::GetAverageLocationMarkQuery(1));
    
            ASSERT_NE(average_mark, nullptr);
            ASSERT_EQ(*average_mark, average_mark_of_first_location);
        }
        
        {
            const auto average_mark =
                    data_storage_.GetAverageLocationMark(
                        DataStorage::GetAverageLocationMarkQuery(2));
    
            ASSERT_NE(average_mark, nullptr);
            ASSERT_EQ(*average_mark, average_mark_of_second_location);
        }
    }
};

TEST_F(DataStorageComplexTest, CheckVisitsByUserIdAfterVisitAddTest)
{
    LoadData("CheckVisitsByUserIdAfterVisitAddTest/");

    {
        const auto visits =
                data_storage_.GetVisistsByUserId(
                    DataStorage::GetVisistsByUserIdQuery(1));

        ASSERT_NE(visits, nullptr);
        AssertEqualJsonDocuments(
                *visits,
                R"({"visits": [{"place": "Поместье", "visited_at": 1049447314, "mark": 3},
                {"place": "Поместье", "visited_at": 1049447316, "mark": 4}]})");
    }

    data_storage_.AddVisit(
            Visit(3, 22, 1, 123, 4));

    {
        const auto visits =
                data_storage_.GetVisistsByUserId(
                    DataStorage::GetVisistsByUserIdQuery(1));

        ASSERT_NE(visits, nullptr);
        AssertEqualJsonDocuments(
                *visits,
                R"({"visits": [{"place": "Поместье", "visited_at": 123, "mark": 4},
                {"place": "Поместье", "visited_at": 1049447314, "mark": 3},
                {"place": "Поместье", "visited_at": 1049447316, "mark": 4}]})");
    }
}

TEST_F(DataStorageComplexTest, CheckAverageLocationAfterVisitAddTest)
{
    LoadData("CheckAverageLocationAfterVisitAddTest/");
    
    {
        const auto visits =
                data_storage_.GetAverageLocationMark(
                    DataStorage::GetAverageLocationMarkQuery(1));

        ASSERT_NE(visits, nullptr);
        ASSERT_EQ(*visits, R"({"avg":3.50000})");
    }
    
    data_storage_.AddVisit(Visit(4, 1, 2, 123, 3));

    {
        const auto visits =
                data_storage_.GetAverageLocationMark(
                    DataStorage::GetAverageLocationMarkQuery(1));

        ASSERT_NE(visits, nullptr);
        ASSERT_EQ(*visits, R"({"avg":3.33333})");
    }
}

TEST_F(DataStorageComplexTest, CheckVisitsByUserIdAfterVisitUpdateTest)
{
    LoadData("CheckVisitsByUserIdAfterVisitUpdateTest/");

    CheckVisits(
            R"({"visits": 
                [{"place": "Поместье", "visited_at": 1049447314, "mark": 3},
                {"place": "Поместье", "visited_at": 1049447316, "mark": 4}]})",
            R"({"visits": []})");

    data_storage_.UpdateVisit(
            Visit(1, 22, 2, 123, 3));

    CheckVisits(
            R"({"visits":[{"place":"Поместье","visited_at":1049447316,"mark":4}]})",
            R"({"visits":[{"place":"Поместье","visited_at":123,"mark":3}]})");
}

TEST_F(DataStorageComplexTest, CheckAverageLocationAfterVisitUpdateTest)
{
    LoadData("CheckAverageLocationAfterVisitUpdateTest/");

    CheckAverageMark(
            R"({"avg":1.50000})",
            R"({"avg":3.50000})");
    
    data_storage_.UpdateVisit(
            Visit(1, 3, 2, 3, 3));

    data_storage_.UpdateVisit(
            Visit(3, 1, 2, 3, 3));

    data_storage_.DumpData();

    CheckAverageMark(
            R"({"avg":2.50000})",
            R"({"avg":4.00000})");
}