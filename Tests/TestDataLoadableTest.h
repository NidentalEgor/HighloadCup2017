#ifndef TEST_DATA_LOADABLE_TEST_H_INLUDED
#define TEST_DATA_LOADABLE_TEST_H_INLUDED
#pragma once

#include <string>

#include "../DataStorage/DataStorage.h"

class TestDataLoadableTest
{
public:
    TestDataLoadableTest(
            const std::string& test_data_folder_path)
        : data_storage_(std::make_unique<DataStorageNew>())
        , test_data_folder_path_(test_data_folder_path)
    {
    }

    virtual ~TestDataLoadableTest() = default;

protected:
    void LoadData(const std::string& test_data_folder_name)
    {
        data_storage_->LoadData(
                test_data_folder_path_ + test_data_folder_name);
    }

protected:
    // DataStorage data_storage_;
    std::unique_ptr<IDataStorage> data_storage_;
    const std::string test_data_folder_path_;
};

#endif // TEST_DATA_LOADABLE_TEST_H_INLUDED