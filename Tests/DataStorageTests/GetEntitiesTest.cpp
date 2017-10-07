#include "gtest/gtest.h"

#include "DataStorage.h"

class DataStorageGetEntitiesTest : public testing::Test
{
protected:
virtual void SetUp()
{
    data_storage_.LoadData("/home/egor/Repositories/HighloadCup2017/Tests/DataStorageTests/TestData/ReadData/");
}

protected:
    DataStorage data_storage_;
};

TEST_F(DataStorageGetEntitiesTest, ReadData)
{
    ASSERT_EQ(data_storage_.GetUsersAmount(), 1);
    ASSERT_EQ(data_storage_.GetLocationsAmount(), 1);
    ASSERT_EQ(data_storage_.GetVisitsAmount(), 1);
}

TEST_F(DataStorageGetEntitiesTest, GetLocation)
{
    const auto location = data_storage_.GetLocationById(22);

    ASSERT_NE(location, nullptr);
    ASSERT_EQ(*location, R"({"id":22,"place":"Поместье","country":"Белоруссия","city":"Белстан","distance":69})");
}

TEST_F(DataStorageGetEntitiesTest, GetUser)
{
    const auto user = data_storage_.GetUserById(27);

    ASSERT_NE(user, nullptr);
    ASSERT_EQ(*user, R"({"id":27,"email":"coghueserriletesik@icloud.com","first_name":"Инна","last_name":"Данатоная","gender":"f","birth_date":-631065600})");
}

TEST_F(DataStorageGetEntitiesTest, GetVisit)
{
    const auto visit = data_storage_.GetVisitById(1);

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"id":1,"location":22,"user":27,"visited_at":1049447314,"mark":3})");
}

TEST_F(DataStorageGetEntitiesTest, GetVisitByUserId)
{
    const auto visit = data_storage_.GetVisistsByUserId(27);

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"visits":[{"id":1,"location":22,"user":27,"visited_at":1049447314,"mark":3}]})");
}

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}