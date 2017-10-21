#ifndef DATA_STORAGE_H_INCLUDED
#define DATA_STORAGE_H_INCLUDED
#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <limits>

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
    using MappedMultiIndexes = std::unordered_map<uint32_t, std::multimap<Timestamp, uint32_t>>;

    enum class UpdateEntityStatus
    {
        EntityNotFound = 0,
        EntitySuccessfullyUpdated
    };

    struct GetAverageLocationMarkQuery
    {
    public:
        GetAverageLocationMarkQuery(
                const uint32_t id)
            : id(id)
        {
        }

    public:
        uint32_t id = 0;
        Timestamp from_date = std::numeric_limits<Timestamp>::min();
        Timestamp to_date = std::numeric_limits<Timestamp>::max();
        Timestamp from_age = std::numeric_limits<Timestamp>::min();
        Timestamp to_age = std::numeric_limits<Timestamp>::max();
        Gender gender = Gender::Any;
    };

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
            const uint32_t to_distance = std::numeric_limits<uint32_t>::max()) const; // think

    std::unique_ptr<std::string> GetAverageLocationMark(
            const GetAverageLocationMarkQuery query_description) const;

    UpdateEntityStatus UpdateUser(
            const User& user);

    UpdateEntityStatus UpdateVisit(
            const Visit& visit);

    UpdateEntityStatus UpdateLocation(
            const Location& location);

    ///
    void DumpData() const;
    ///

private:
    void MapEntities();
///
public:
///
    Timestamp GetBoundaryBirthDate(
            const short age) const;

    template <typename T>
    std::unique_ptr<std::string> GetEntityById(
            const uint32_t entity_id,
            const Container<T>& entities);

    template <typename T>
    DataStorage::UpdateEntityStatus UpdateEntity(
            const T& entity,
            Container<T>& entities);

    template <typename T>
    void ParseFile(
            const std::string& file_path,
            const std::string& entities_name,            
            Container<T>& container);

    template <typename T>
    void EraseLessOrEqualElements(
            const Timestamp bound,
            T& container) const;

    template <typename T>
    void EraseGreaterOrEqualElements(
            const Timestamp bound,
            T& container) const;

    template <typename Comparator>
    void EraseByAge(
            const Timestamp from_age,
            Comparator comparator,
            std::multimap<Timestamp, uint32_t>& visits) const;

    void EraseByGender(
            const Gender gender,
            std::multimap<Timestamp, uint32_t>& visits) const;

private:
    Container<Location> locations_;
    Container<User> users_;
    Container<Visit> visits_;
    MappedIndexes visits_to_locations_;
    MappedIndexes visits_to_user_;
    MappedMultiIndexes users_to_visits_;
    MappedMultiIndexes locations_to_visits_;
    MappedMultiIndexes locations_to_users_;
};

#endif // DATA_STORAGE_H_INCLUDED