#include "gtest/gtest.h"

#include "../../TimeMeasurableTest.h"
#include "../../TestDataLoadableTest.h"
#include "../../JsonComparableTest.h"

class AddEntityTest
    : public TimeMeasurableTest
    , public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    AddEntityTest()
        : TestDataLoadableTest("")
    {
    }
};

TEST_F(AddEntityTest, SimpleAddVisitTest)
{
    ASSERT_EQ(
        data_storage_->AddVisit(Visit(1, 2, 3, 769572247, 4)),
        IDataStorage::AddEntityStatus::EntitySuccessfullyAdded);
    
    ASSERT_EQ(
        data_storage_->GetVisitsAmount(),
        1);
    // Check that entity was added correctly...
}

TEST_F(AddEntityTest, AddAlreadyExistedVisitTest)
{
    data_storage_->AddVisit(
            Visit(1, 2, 3, 769572247, 4));
    
    ASSERT_EQ(
        data_storage_->AddVisit(Visit(1, 2, 3, 769572247, 4)),
        IDataStorage::AddEntityStatus::EntityAlreadyExist);

    ASSERT_EQ(
        data_storage_->AddVisit(Visit(2, 2, 3, 769572247, 4)),
        IDataStorage::AddEntityStatus::EntitySuccessfullyAdded);

    ASSERT_EQ(
        data_storage_->GetVisitsAmount(),
        2);
}

TEST_F(AddEntityTest, SimpleAddLocationTest)
{
    ASSERT_EQ(
        data_storage_->AddLocation(Location(1, "p", "c", "c", 123)),
        IDataStorage::AddEntityStatus::EntitySuccessfullyAdded);
    
    ASSERT_EQ(
        data_storage_->GetLocationsAmount(),
        1);
}

TEST_F(AddEntityTest, AddAlreadyExistedLocationTest)
{
    data_storage_->AddLocation(Location(1, "p", "c", "c", 123));
    
    ASSERT_EQ(
        data_storage_->AddLocation(Location(1, "p", "c", "c", 123)),
        IDataStorage::AddEntityStatus::EntityAlreadyExist);

    ASSERT_EQ(
        data_storage_->AddLocation(Location(2, "p", "c", "c", 123)),
        IDataStorage::AddEntityStatus::EntitySuccessfullyAdded);

    ASSERT_EQ(
        data_storage_->GetLocationsAmount(),
        2);
}

TEST_F(AddEntityTest, SimpleAddUserTest)
{
    data_storage_->AddUser(
            User(1, "m", "e", "n", Gender::Male, 769572247));

    ASSERT_EQ(
        data_storage_->GetUsersAmount(),
        1);
}

TEST_F(AddEntityTest, AddAlreadyExistedUserTest)
{
    data_storage_->AddUser(
            User(1, "m", "e", "n", Gender::Male, 769572247));
    
    ASSERT_EQ(
        data_storage_->AddUser(
            User(1, "m", "e", "n", Gender::Male, 769572247)),
        IDataStorage::AddEntityStatus::EntityAlreadyExist);

    ASSERT_EQ(
        data_storage_->AddUser(
            User(2, "m", "e", "n", Gender::Male, 769572247)),
        IDataStorage::AddEntityStatus::EntitySuccessfullyAdded);

    ASSERT_EQ(
        data_storage_->GetUsersAmount(),
        2);
}