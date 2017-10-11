#include "gtest/gtest.h"
#include "rapidjson/document.h"

#include "DataStorage.h"
#include "../../TestDataLoadableTest.h"

class UpdateEntityTests
    : public TestDataLoadableTest
    , public testing::Test
{
public:
    UpdateEntityTests()
        : TestDataLoadableTest(R"(/home/egor/Repositories/HighloadCup2017/)"
            R"(Tests/DataStorageTests/UpdateEntityTests/TestData/)")
    {
    }
};

TEST_F(UpdateEntityTests, UpdateUserWhichNotExistTest)
{
    LoadData("UpdateUserWhichNotExistTest/");

    data_storage_.DumpData();

    User user(
        123,
        "new_email@mymail.ru",
        "new_first_name",
        "new_last_name",
        Gender::Female,
        123456);

    const auto result = data_storage_.UpdateUser(user);
    ASSERT_EQ(result, DataStorage::UpdateEntityStatus::EntityNotFound);
}