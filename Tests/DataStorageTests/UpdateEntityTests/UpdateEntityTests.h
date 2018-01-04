#include "gtest/gtest.h"
#include "../../../Submodules/rapidjson/include/rapidjson/document.h"

#include "../../TimeMeasurableTest.h"
#include "../../TestDataLoadableTest.h"
#include "../../JsonComparableTest.h"

class UpdateEntityTests
    : public TestDataLoadableTest
    , public JsonComparableTest
    , public testing::Test
{
public:
    UpdateEntityTests()
        : TestDataLoadableTest(R"(/home/egor/Repositories/highloadcup2017/)"
            R"(Tests/DataStorageTests/UpdateEntityTests/TestData/)")
        , entity_id(1234)
    {
    }

protected:
    User GetUserToUpdate() const
    {
        return User(
            entity_id,
            "new_email@mymail.ru",
            "new_first_name",
            "new_last_name",
            Gender::Female,
            123456);
    }

    Visit GetVisitToUpdate() const
    {
        return Visit(
            entity_id,
            123,
            123,
            123,
            123);
    }

    Location GetLocationToUpdate() const
    {
        return Location(
            entity_id,
            "123",
            "123",
            "123",
            123);
    }

protected:
    size_t entity_id;
};

TEST_F(UpdateEntityTests, UpdateNonexistentEntitiesTest)
{
    LoadData("UpdateNonExistentEntitiesTest/");

    // Test for user.
    const size_t users_amount_before_update =
            data_storage_->GetUsersAmount();
    const auto user_update_result =
            data_storage_->UpdateUser(GetUserToUpdate());
    ASSERT_EQ(users_amount_before_update, data_storage_->GetUsersAmount());
    ASSERT_EQ(user_update_result, IDataStorage::UpdateEntityStatus::EntityNotFound);

    // Test for visit.
    const size_t visits_amount_before_update =
            data_storage_->GetVisitsAmount();
    const auto visit_update_result =
            data_storage_->UpdateVisit(GetVisitToUpdate());
    ASSERT_EQ(visits_amount_before_update, data_storage_->GetVisitsAmount());
    ASSERT_EQ(visit_update_result, IDataStorage::UpdateEntityStatus::EntityNotFound);

    // Test for location.
    const size_t locations_amount_before_update =
            data_storage_->GetLocationsAmount();
    const auto location_update_result =
            data_storage_->UpdateLocation(GetLocationToUpdate());
    ASSERT_EQ(locations_amount_before_update, data_storage_->GetLocationsAmount());
    ASSERT_EQ(location_update_result, IDataStorage::UpdateEntityStatus::EntityNotFound);
}

TEST_F(UpdateEntityTests, UpdateExistingUserTest)
{
    LoadData("UpdateExistingUserTest/");

    const size_t users_amount_before_update =
            data_storage_->GetUsersAmount();

    const auto user_update_result =
            data_storage_->UpdateUser(GetUserToUpdate());
    ASSERT_EQ(users_amount_before_update, data_storage_->GetUsersAmount());
    ASSERT_EQ(user_update_result, IDataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated);

    const auto updated_user = data_storage_->GetUserById(entity_id);
    ASSERT_NE(updated_user, nullptr);
    AssertEqualJsonDocuments(
            *updated_user,
            R"({"id":1234,"email":"new_email@mymail.ru","first_name":"new_first_name",)"
            R"("last_name":"new_last_name","gender":"f","birth_date":123456})");
}

TEST_F(UpdateEntityTests, UpdateExistingVisitTest)
{
    LoadData("UpdateExistingVisitTest/");
    
    const size_t visits_amount_before_update =
            data_storage_->GetVisitsAmount();

    const auto visit_update_result =
            data_storage_->UpdateVisit(GetVisitToUpdate());
    ASSERT_EQ(visits_amount_before_update, data_storage_->GetVisitsAmount());
    ASSERT_EQ(visit_update_result, IDataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated);

    const auto updated_visit = data_storage_->GetVisitById(entity_id);
    ASSERT_NE(updated_visit, nullptr);

    AssertEqualJsonDocuments(
            *updated_visit,
            R"({"user": 123, "location": 123, "visited_at": 123, "id": 1234, "mark": 123})");
}

TEST_F(UpdateEntityTests, UpdateExistingLocationTest)
{
    LoadData("UpdateExistingLocationTest/");
    
    const size_t locations_amount_before_update =
            data_storage_->GetLocationsAmount();

    const auto location_update_result =
            data_storage_->UpdateLocation(GetLocationToUpdate());
    ASSERT_EQ(locations_amount_before_update, data_storage_->GetLocationsAmount());
    ASSERT_EQ(location_update_result, IDataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated);

    const auto updated_location = data_storage_->GetLocationById(entity_id);
    ASSERT_NE(updated_location, nullptr);
    AssertEqualJsonDocuments(
            *updated_location,
            R"({"distance": 123, "city": "123", "place": "123", "id": 1234,)"
            R"( "country": "123"})");
}