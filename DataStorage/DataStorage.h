#ifndef DATA_STORAGE_H_INCLUDED
#define DATA_STORAGE_H_INCLUDED
#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <memory>
#include <limits>

#include "../Utils/Traceable.h"
#include "Entities/DataTypes.h"
#include "Entities/Location.h"
#include "Entities/Visit.h"
#include "Entities/User.h"

class DataStorage final
        : public Traceable
{
public:
    template <typename T>
    using Container = std::unordered_map<Id, T>;
    using MappedIndexes = std::unordered_map<Id, Id>;
    using TimestampToId = std::multimap<Timestamp, Id>;
    using MappedMultiIndexes = std::unordered_map<Id, TimestampToId>;

    enum class UpdateEntityStatus
    {
        EntityNotFound = 0,
        EntitySuccessfullyUpdated
    };

    enum class AddEntityStatus
    {
        EntityAlreadyExist = 0,
        EntitySuccessfullyAdded
    };

    struct GetAverageLocationMarkQuery
    {
    public:
        explicit GetAverageLocationMarkQuery(
                const Id id)
            : id(id)
        {
        }

    public:
        Id id = 0;
        Timestamp from_date = std::numeric_limits<Timestamp>::min();
        Timestamp to_date = std::numeric_limits<Timestamp>::max();
        Timestamp from_age = std::numeric_limits<Timestamp>::min();
        Timestamp to_age = std::numeric_limits<Timestamp>::max();
        Gender gender = Gender::Any;
    };

    struct GetVisistsByUserIdQuery
    {
    public:
        explicit GetVisistsByUserIdQuery(
                const Id id)
            : id(id)
        {
        }

    public:
        Id id;
        Timestamp from_date = std::numeric_limits<Timestamp>::min();
        Timestamp to_date = std::numeric_limits<Timestamp>::max();
        std::string country;
        Distance to_distance = std::numeric_limits<Distance>::max();
    };

public:
    void LoadData(
            const std::string& folder_path);
    
    void LoadZippedData(
            const std::string& path_to_zipped_data);

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
            const Id location_id) const;

    std::unique_ptr<std::string> GetUserById(
            const Id user_id) const;

    std::unique_ptr<std::string> GetVisitById(
            const Id visit_id) const;

    template <typename T>        
    std::unique_ptr<std::string> GetEntityById(
            const Id entity_id);

    std::unique_ptr<std::string> GetVisistsByUserId(
            const GetVisistsByUserIdQuery& query_description) const;

    std::unique_ptr<std::string> GetAverageLocationMark(
            const GetAverageLocationMarkQuery query_description) const;

    template<typename T>
    UpdateEntityStatus UpdateEntity(
            const T& entity);

    UpdateEntityStatus UpdateUser(
            const User& user);

    UpdateEntityStatus UpdateVisit(
            const Visit& visit);

    UpdateEntityStatus UpdateLocation(
            const Location& location);

    AddEntityStatus AddUser(
            User&& user);

    AddEntityStatus AddVisit(
            Visit&& visit);

    AddEntityStatus AddLocation(
            Location&& location);

    template<typename T>
    AddEntityStatus AddEntity(
            T&& entity);

    void DumpData() const;

private:
    void InitializeUsers(
            const std::string& path_to_zipped_data);

    void InitializeLocations(
            const std::string& path_to_zipped_data);

    void InitializeVisits(
            const std::string& path_to_zipped_data);

    void MapEntities();

    Timestamp GetBoundaryBirthDate(
            const Age age) const;

    template <typename T>
    std::unique_ptr<std::string> GetEntityById(
            const Id entity_id,
            const Container<T>& entities) const;

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
            std::multimap<Timestamp, Id>& visits) const;

    void EraseByGender(
            const Gender gender,
            std::multimap<Timestamp, Id>& visits) const;

    void EraseByCountry(
            const std::string& country,
            std::multimap<Timestamp, Id>& visits) const;
    
    void EraseByToDistance(
            const Distance to_distance,
            std::multimap<Timestamp, Id>& visits) const;

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