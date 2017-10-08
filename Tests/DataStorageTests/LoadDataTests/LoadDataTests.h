#include "gtest/gtest.h"

#include "DataStorage.h"

class DataStorageLoadDataTest : public testing::Test
{
protected:
    void LoadData(const std::string& test_data_folder_name)
    {
        data_storage_.LoadData(test_data_folder_path_ + test_data_folder_name);
    }

    void AssertEntitiesAmount(const size_t entities_amount)
    {
        ASSERT_EQ(data_storage_.GetLocationsAmount(), entities_amount);
        ASSERT_EQ(data_storage_.GetVisitsAmount(),entities_amount);
        ASSERT_EQ(data_storage_.GetUsersAmount(),entities_amount);
    }

protected:
    DataStorage data_storage_;
    // Temp solution.
    // std::string test_data_folder_path_ = R"(../../../../Tests/DataStorageTests/LoadDataTests/TestData/)";
    std::string test_data_folder_path_ = R"(/home/egor/Repositories/HighloadCup2017/Tests/DataStorageTests/LoadDataTests/TestData/)";
    // Temp solution.
};

TEST_F(DataStorageLoadDataTest, NoDataTest)
{
    LoadData("NoDataTest/");

    AssertEntitiesAmount(0);
}

TEST_F(DataStorageLoadDataTest, NullExemplarOfEachEntityTest)
{
    LoadData("NullExemplarOfEachEntityTest/");

    AssertEntitiesAmount(0);
}

TEST_F(DataStorageLoadDataTest, OneExemplarOfEachEntityTest)
{
    LoadData("OneExemplarOfEachEntityTest/");

    AssertEntitiesAmount(1);
}

TEST_F(DataStorageLoadDataTest, TwoExemplarOfEachEntityTest)
{
    LoadData("TwoExemplarOfEachEntityTest/");

    AssertEntitiesAmount(2);
}
