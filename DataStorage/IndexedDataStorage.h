#ifndef INDEXED_DATA_STORAGE_H_INCLUDED
#define INDEXED_DATA_STORAGE_H_INCLUDED
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
#include "IDataStorage.h"

class IndexedDataStorage final
        : public IDataStorage
        , public Traceable
{
public:
    template <typename T>
    using Container = std::unordered_map<Id, T>;
    using MappedIndexes = std::unordered_map<Id, Id>;
    using TimestampToId = std::multimap<Timestamp, Id>;
    using MappedMultiIndexes = std::unordered_map<Id, TimestampToId>;

public:
    void LoadData(
            const std::string& folder_path) override;
    
    void LoadZippedData(
            const std::string& path_to_zipped_data) override;

    size_t GetLocationsAmount() override
    {
        return locations_.size();
    }

    size_t GetVisitsAmount() override
    {
        return visits_.size();
    }

    size_t GetUsersAmount() override
    {
        return users_.size();
    }

    std::unique_ptr<std::string> GetLocationById(
            const Id location_id) const override;

    std::unique_ptr<std::string> GetUserById(
            const Id user_id) const override;

    std::unique_ptr<std::string> GetVisitById(
            const Id visit_id) const override;

    template <typename T>        
    std::unique_ptr<std::string> GetEntityById(
            const Id entity_id);

    std::unique_ptr<std::string> GetVisistsByUserId(
            GetVisistsByUserIdQuery&& query_description) const override;

    std::unique_ptr<std::string> GetAverageLocationMark(
            GetAverageLocationMarkQuery&& query_description) const override;

    template<typename T>
    UpdateEntityStatus UpdateEntity(
            const T& entity);

    UpdateEntityStatus UpdateUser(
            const User& user) override;

    UpdateEntityStatus UpdateVisit(
            const Visit& visit) override;

    UpdateEntityStatus UpdateLocation(
            const Location& location) override;

    AddEntityStatus AddUser(
            User&& user) override;

    AddEntityStatus AddVisit(
            Visit&& visit) override;

    AddEntityStatus AddLocation(
            Location&& location) override;

    template<typename T>
    AddEntityStatus AddEntity(
            T&& entity);

    void DumpData() const override;

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

#endif // INDEXED_DATA_STORAGE_H_INCLUDED