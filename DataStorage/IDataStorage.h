#ifndef I_DATA_STORAGE_H_INCLUDED
#define I_DATA_STORAGE_H_INCLUDED
#pragma once

#include "Entities/Location.h"
#include "Entities/Visit.h"
#include "Entities/User.h"

class IDataStorage
{
public:
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
    virtual void LoadData(
            const std::string& folder_path) = 0;
    
    virtual void LoadZippedData(
            const std::string& path_to_zipped_data) = 0;

    virtual size_t GetLocationsAmount() = 0;

    virtual size_t GetVisitsAmount() = 0;

    virtual size_t GetUsersAmount() = 0;

    virtual std::unique_ptr<std::string> GetLocationById(
            const Id location_id) const = 0;

    virtual std::unique_ptr<std::string> GetUserById(
            const Id user_id) const = 0;

    virtual std::unique_ptr<std::string> GetVisitById(
            const Id visit_id) const = 0;

    template <typename T>        
    std::unique_ptr<std::string> ProcessGetEntityByIdRequest(
            const Id entity_id);

    virtual std::unique_ptr<std::string> GetVisistsByUserId(
            const GetVisistsByUserIdQuery& query_description) const = 0;

    virtual std::unique_ptr<std::string> GetAverageLocationMark(
            const GetAverageLocationMarkQuery query_description) const = 0;

    template<typename T>
    UpdateEntityStatus UpdateEntity(
            const T& entity);

    virtual UpdateEntityStatus UpdateUser(
            const User& user) = 0;

    virtual UpdateEntityStatus UpdateVisit(
            const Visit& visit) = 0;

    virtual UpdateEntityStatus UpdateLocation(
            const Location& location) = 0;

    virtual AddEntityStatus AddUser(
            User&& user) = 0;

    virtual AddEntityStatus AddVisit(
            Visit&& visit) = 0;

    virtual AddEntityStatus AddLocation(
            Location&& location) = 0;
    
    template<typename T>
    AddEntityStatus AddEntity(
            T&& entity);
};

#endif // I_DATA_STORAGE_H_INCLUDED