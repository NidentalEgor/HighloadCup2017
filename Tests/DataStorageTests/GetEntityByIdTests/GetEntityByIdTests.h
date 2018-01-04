#include "gtest/gtest.h"

#include "../../TimeMeasurableTest.h"
#include "../../DataStorage/DataStorage.h"

class DataStorageGetEntitiesTest
    : public testing::Test
    , public TimeMeasurableTest
{
public:
    static void SetUpTestCase()
    {
        data_storage_->LoadData("/home/egor/Repositories/highloadcup2017/Tests/DataStorageTests/GetEntityByIdTests/TestData/");
    }

protected:
    static std::unique_ptr<IDataStorage> data_storage_;
};

std::unique_ptr<IDataStorage> DataStorageGetEntitiesTest::data_storage_ =
        std::make_unique<DataStorageNew>();

TEST_F(DataStorageGetEntitiesTest, NonExistingLocationTest)
{
    ASSERT_EQ(DataStorageGetEntitiesTest::data_storage_->GetLocationById(25), nullptr);
}

TEST_F(DataStorageGetEntitiesTest, ExistingLocationTest)
{
    const auto location = DataStorageGetEntitiesTest::data_storage_->GetLocationById(22);

    ASSERT_NE(location, nullptr);
    ASSERT_EQ(*location, R"({"id":22,"place":"Поместье","country":"Белоруссия","city":"Белстан","distance":69})");
}

TEST_F(DataStorageGetEntitiesTest, NonExistingVisitTest)
{
    ASSERT_EQ(DataStorageGetEntitiesTest::data_storage_->GetVisitById(3), nullptr);
}

TEST_F(DataStorageGetEntitiesTest, ExistingVisitTest)
{
    const auto visit = DataStorageGetEntitiesTest::data_storage_->GetVisitById(1);

    ASSERT_NE(visit, nullptr);
    ASSERT_EQ(*visit, R"({"id":1,"location":22,"user":27,"visited_at":1049447316,"mark":4})");
}

TEST_F(DataStorageGetEntitiesTest, NonExistingUserTest)
{
    ASSERT_EQ(DataStorageGetEntitiesTest::data_storage_->GetUserById(1), nullptr);
}

TEST_F(DataStorageGetEntitiesTest, ExistingUserTest)
{
    const auto user = DataStorageGetEntitiesTest::data_storage_->GetUserById(27);

    ASSERT_NE(user, nullptr);
    ASSERT_EQ(*user, R"({"id":27,"email":"coghueserriletesik@icloud.com","first_name":"Инна","last_name":"Данатоная","gender":"f","birth_date":-631065600})");
}
