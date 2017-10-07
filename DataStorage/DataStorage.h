#ifndef DATA_STORAGE_H_INCLUDED
#define DATA_STORAGE_H_INCLUDED
#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <memory>

#include "Entities/Location.h"
#include "Entities/User.h"
#include "Entities/Visit.h"
#include "Entities/DataTypes.h"

class DataStorage
{
public:
    template <typename T>
    using Container = std::unordered_map<uint32_t, T>;
    using MappedIndexes = std::unordered_map<uint32_t, uint32_t>;
    using MappedMultiIndexes = std::unordered_map<uint32_t, std::map<Timestamp, uint32_t>>;    

public:
    void LoadData(
            const std::string& folder_path);
    
    size_t GetLocationsAmount()
    {
        return locations_.size();
    }

    size_t GetVisitsAmount()
    {
        return visits_.size();
    }

    size_t GetUsersAmount()
    {
        return users_.size();
    }

    std::unique_ptr<std::string> GetLocationById(
            const uint32_t location_id);

    std::unique_ptr<std::string> GetUserById(
            const uint32_t user_id);

    std::unique_ptr<std::string> GetVisitById(
            const uint32_t visit_id);

    std::unique_ptr<std::string> GetVisistsByUserId(
            const uint32_t user_id,
            const Timestamp from_date = -1,
            const Timestamp to_date = -1,
            const std::string& country = "",
            const uint32_t to_distance = 0); // think
    ///
    void DumpData();
    ///

private:
    void MapEntities();

    template <typename T>
    std::unique_ptr<std::string> GetEntityById(
            const uint32_t entity_id,
            const Container<T>& entities);

    template <typename T>
    void ParseFile(
            const std::string& file_path,
            const std::string& entities_name,            
            Container<T>& container);

private:
    Container<Location> locations_;
    Container<User> users_;
    Container<Visit> visits_;
    MappedIndexes visites_to_locations_;
    MappedIndexes visites_to_users_;
    MappedMultiIndexes users_to_visits_;
};

#endif // DATA_STORAGE_H_INCLUDED