#include <dirent.h>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "Macroses.h"
#include "DataStorage.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"
#include <cstdio>

void DataStorage::LoadData(const std::string& folder_path)
{
    DIR* directory;
    struct dirent* entity;
    if ((directory = opendir(folder_path.c_str())) != nullptr)
    {
        while ((entity = readdir (directory)) != nullptr)
        {
            const std::string file_path =
                    folder_path + std::string(entity->d_name);
            switch (entity->d_name[0])
            {
                case 'l':
                {
                    ParseFile(file_path, "locations", locations_);
                } break;

                case 'u':
                {
                    ParseFile(file_path, "users", users_);
                } break;

                case 'v':
                {
                    ParseFile(file_path, "visits", visits_);
                } break;
            }
        }
        closedir(directory);
    }
    else
    {
        ///
        perror ("");
        ///
    }

    MapEntities();
}

template <typename T>
void DataStorage::ParseFile(
        const std::string& file_path,
        const std::string& entities_name,
        Container<T>& container)
{
    using namespace rapidjson;

    FILE* file =
    fopen(file_path.c_str(), "r");
    char buffer[65536];
    FileReadStream file_read_stream(
        file,
        buffer,
        sizeof(buffer));

    Document entities;
    entities.ParseStream(file_read_stream);

    fclose(file);
    
    const Value& data = entities[entities_name.c_str()];
    assert(data.IsArray());

    for (const auto& entity : data.GetArray())
    {
        T element;
        element.Deserialize(entity);
        container.emplace(element.GetId(), element);
    }
}

void DataStorage::MapEntities()
{
    for (const auto& visit : visits_)
    {
        const auto location =
                locations_.find(visit.second.location_id_);
        const auto user =
                users_.find(visit.second.user_id_);
        if (location != locations_.end() &&
            user != users_.end())
        {
            visits_to_locations_.emplace(visit.first, location->first);
            locations_to_visits_[location->first].emplace(visit.second.visited_at_, visit.first);
            locations_to_users_[location->first].emplace(user->second.birth_date_, user->first);
            visits_to_user_.emplace(visit.first, user->first);
            users_to_visits_[user->first].emplace(visit.second.visited_at_, visit.first);
        }
        else
        {
            ///
            // Error!!!
            ///
        }


    }
}

std::unique_ptr<std::string> DataStorage::GetLocationById(
        const uint32_t location_id)
{
    return GetEntityById<Location>(location_id, locations_);
}

std::unique_ptr<std::string> DataStorage::GetUserById(
        const uint32_t user_id)
{
    return GetEntityById<User>(user_id, users_);
}

std::unique_ptr<std::string> DataStorage::GetVisitById(
        const uint32_t visit_id)
{
    return GetEntityById<Visit>(visit_id, visits_);
}

template <typename T>
std::unique_ptr<std::string> DataStorage::GetEntityById(
        const uint32_t entity_id,
        const Container<T>& entities)
{
    const auto entity = entities.find(entity_id);
            
    return
        entity == entities.end()
        ? nullptr
        : std::make_unique<std::string>(*entity->second.Serialize());
}

void DataStorage::EraseByCountry(
        const std::string& country,
        std::multimap<Timestamp, uint32_t>& visits) const
{
    auto visit_description = visits.begin();
    while (visit_description != visits.end())
    {
        const auto visit_id_to_location_id =
                visits_to_locations_.find(visit_description->second);
        if (visit_id_to_location_id == visits_to_locations_.end())
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        const auto location =
                locations_.find(visit_id_to_location_id->second);
        if (location->second.country_ != country)
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        ++visit_description;
    }
}

void DataStorage::EraseByToDistance(
        const uint32_t to_distance,
        std::multimap<Timestamp, uint32_t>& visits) const
{
    auto visit_description = visits.begin();
    while (visit_description != visits.end())
    {
        const auto visit_id_to_location_id =
                visits_to_locations_.find(visit_description->second);
        if (visit_id_to_location_id == visits_to_locations_.end())
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        const auto location =
                locations_.find(visit_id_to_location_id->second);
        if (location->second.distance_ >= to_distance)
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        ++visit_description;
    }
}

std::unique_ptr<std::string> DataStorage::GetVisistsByUserId(
        const GetVisistsByUserIdQuery& query_description) const
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(query_description.id) != users_.end(),
            nullptr);

    const auto visits_iterator =
            users_to_visits_.find(query_description.id);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            visits_iterator != users_to_visits_.end(),
            std::make_unique<std::string>(R"({"visits":[]})"));

    auto visits = visits_iterator->second;

    if (query_description.from_date != std::numeric_limits<Timestamp>::min())
    {
        EraseLessOrEqualElements(query_description.from_date, visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"visits":[]})"));
    }

    if (query_description.to_date != std::numeric_limits<Timestamp>::max())
    {
        EraseGreaterOrEqualElements(query_description.to_date, visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"visits":[]})"));
    }

    if (query_description.country != "") // may be diffrent mark?
    {
        EraseByCountry(
                query_description.country,
                visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"visits":[]})"));
    }

    if (query_description.to_distance != std::numeric_limits<uint32_t>::max())
    {
        EraseByToDistance(
                query_description.to_distance,
                visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"visits":[]})"));
    }

    std::string result(R"({"visits":[)");
    for (const auto visit_description : visits)
    {
        result += *visits_.find(visit_description.second)->second.Serialize();
        result += ',';
    }

    if (!visits.empty())
    {
        result.pop_back();
    }

    result += "]}";
    return std::make_unique<std::string>(result);
}

template <typename T>
void DataStorage::EraseGreaterOrEqualElements(
    const Timestamp bound,
    T& container) const
{
    container.erase(
            container.lower_bound(bound),
            container.end());
}

template <typename T>
void DataStorage::EraseLessOrEqualElements(
        const Timestamp bound,
        T& container) const
{
    container.erase(
            container.begin(),
            container.upper_bound(bound));
}

template <typename Comparator>
void DataStorage::EraseByAge(
        const Timestamp from_age,
        Comparator comparator,
        std::multimap<Timestamp, uint32_t>& visits) const
{   
    const auto from_age_date =
            GetBoundaryBirthDate(from_age);

    auto current_visit = visits.begin();
    while (current_visit != visits.end())
    {
        auto visit_to_user =
                visits_to_user_.find(current_visit->second);
        
        if (visit_to_user == visits_to_user_.end())
        {
            current_visit = visits.erase(current_visit);
            continue;
        }

        auto current_user =
                users_.find(visit_to_user->second);

        if (current_user == users_.end() ||
            comparator(current_user->second.birth_date_, from_age_date))
        {
            current_visit = visits.erase(current_visit);
            continue;
        }

        ++current_visit;
    }
}

void DataStorage::EraseByGender(
        const Gender gender,
        std::multimap<Timestamp, uint32_t>& visits) const
{
    auto current_visit = visits.begin();
    while (current_visit != visits.end())
    {
        auto visit_to_user =
                visits_to_user_.find(current_visit->second);
        if (visit_to_user == visits_to_user_.end())
        {
            current_visit = visits.erase(current_visit);
            continue;
        }

        auto user = users_.find(visit_to_user->second);
        if (user == users_.end() ||
            gender != user->second.gender_)
        {
            current_visit = visits.erase(current_visit);
            continue;
        }

        ++current_visit;
    }
}

std::unique_ptr<std::string> DataStorage::GetAverageLocationMark(
        const GetAverageLocationMarkQuery query_description) const
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(query_description.id) != locations_.end(),
            nullptr);

    const auto location_id_to_visits =
            locations_to_visits_.find(query_description.id);
    ENSURE_TRUE_OTHERWISE_RETURN(
            location_id_to_visits != locations_to_visits_.end(),
            std::make_unique<std::string>(R"({"avg":0})"));

    auto visits = location_id_to_visits->second;

    if (query_description.from_date != std::numeric_limits<Timestamp>::min())
    {
        EraseLessOrEqualElements(query_description.from_date, visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"avg":0})"));
    }

    if (query_description.to_date != std::numeric_limits<Timestamp>::max())
    {
        EraseGreaterOrEqualElements(query_description.to_date, visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"avg":0})"));  
    }

    auto location_to_users =    
            locations_to_users_.find(query_description.id);
    auto users =
            location_to_users->second;

    if (query_description.from_age != std::numeric_limits<Timestamp>::min())
    {
        EraseByAge(
                query_description.from_age,
                std::greater<Timestamp>(),
                visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"avg":0})"));
    }

    if (query_description.to_age != std::numeric_limits<Timestamp>::max())
    {
        EraseByAge(
                query_description.to_age,
                std::less<Timestamp>(),
                visits);
        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"avg":0})"));
    }

    if (query_description.gender != Gender::Any)
    {
        EraseByGender(
                query_description.gender,
                visits);

        ENSURE_TRUE_OTHERWISE_RETURN(
                !visits.empty(),
                std::make_unique<std::string>(R"({"avg":0})"));
    }

    double sum = 0.0;
    size_t visits_amount = 0;
    for (auto visit : visits)
    {
        const auto& current_visit = visits_.find(visit.second)->second;
        sum += visits_.find(visit.second)->second.mark_;
        ++visits_amount;
    }

    std::stringstream num;
    num << std::fixed << std::setprecision(5) <<
            sum / visits.size();
    std::string result(R"({"avg":)");
    result += num.str();
    result += R"(})";

    return std::make_unique<std::string>(result);
}

Timestamp DataStorage::GetBoundaryBirthDate(
        const short age) const
{
    static time_t now_epoch_time = time(NULL);
    static struct tm* now_time = gmtime(&now_epoch_time);
    auto birth_date = *now_time;
    birth_date.tm_year -= age;
    time_t birth_date_epoch = mktime(&birth_date);

    return birth_date_epoch;
}

DataStorage::UpdateEntityStatus DataStorage::UpdateUser(
        const User& user)
{
    // May I recalc something?
    return UpdateEntity<User>(user, users_);
}

DataStorage::UpdateEntityStatus DataStorage::UpdateVisit(
        const Visit& visit)
{
    // May I recalc something?
    return UpdateEntity<Visit>(visit, visits_);
}

DataStorage::UpdateEntityStatus DataStorage::UpdateLocation(
        const Location& location)
{
    // May I recalc something?
    return UpdateEntity<Location>(location, locations_);
}

template <typename T>
DataStorage::UpdateEntityStatus DataStorage::UpdateEntity(
        const T& entity,
        Container<T>& entities)
{
    const auto entity_to_update = entities.find(entity.id_);

    ENSURE_TRUE_OTHERWISE_RETURN(
            entity_to_update != entities.end(),
            UpdateEntityStatus::EntityNotFound)

    entities[entity.id_] = entity;

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

void DataStorage::AddUser(
        User&& user)
{
    // users_.emplace_back(user);

    const auto user_id_to_visits = users_to_visits_.find(user.id_);
    if (user_id_to_visits != users_to_visits_.end())
    {
        // const auto visits =
        //         users_to_visits_
    }
    else
    {

    }

    // MappedIndexes visits_to_user_;
    // MappedMultiIndexes users_to_visits_;
    // MappedMultiIndexes locations_to_users_;
}

void DataStorage::AddVisit(
        Visit&& visit)
{
    visits_.emplace(visit.id_, visit);

    // MappedIndexes visits_to_locations_; // yes
    // MappedIndexes visits_to_user_; // yes
    // MappedMultiIndexes users_to_visits_; // yes
    // MappedMultiIndexes locations_to_visits_; // yes

    visits_to_user_.emplace(
            visit.id_,
            visit.user_id_);
    visits_to_locations_.emplace(
            visit.id_,
            visit.location_id_);

    const auto location_id_to_visits =
            locations_to_visits_.find(visit.location_id_);
    if (location_id_to_visits == locations_to_visits_.end())
    {
        auto emplaced_element =
                locations_to_visits_.emplace(
                    visit.location_id_,
                    std::multimap<Timestamp, uint32_t>());
        if (emplaced_element.second)
        {
            emplaced_element.first->second.emplace(
                visit.visited_at_,
                visit.id_);
        }
        else
        {
            ///
            // Error!!!
            ///
        }
    }
    else
    {
        ///
        // Error!!!
        ///
    }

    // MappedMultiIndexes users_to_visits_;
    const auto user_id_to_visits =
            users_to_visits_.find(visit.user_id_);
    if (user_id_to_visits != users_to_visits_.end())
    {
        // Try to add enywhere - user can not be in two
        // locations in same time/
        user_id_to_visits->second.emplace(
                visit.visited_at_,
                visit.id_);
    }
    else
    {
        ///
        // Error!!!
        ///
    }
}

void DataStorage::AddLocation(
        Location&& location)
{
    // Container<Location> locations_; // yes
    // MappedIndexes visits_to_locations_; // not nesassary
    // MappedMultiIndexes locations_to_visits_; // yes
    // MappedMultiIndexes locations_to_users_; //

    locations_.emplace(location.id_,location);

    const auto location_id_to_visits =
            locations_to_visits_.find(location.id_);
    if (location_id_to_visits == locations_to_visits_.end())
    {
        locations_to_visits_.emplace(
                location.id_,
                std::multimap<Timestamp,uint32_t>());
    }
    else
    {
        ///
        // Do nothing???
        ///
    }

    const auto location_id_to_users =
            locations_to_users_.find(location.id_);
    if (location_id_to_users == locations_to_users_.end())
    {
        locations_to_users_.emplace(
                location.id_,
                std::multimap<Timestamp, uint32_t>());
    }
    else
    {
        ///
        // Do nothing???
        ///
    }
}

///
#include <fstream>
void DataStorage::DumpData() const
{
    std::ofstream out_loc("visits_to_locations.txt");
    for (const auto& visit_to_location : visits_to_locations_)
    {
        out_loc << "visit_id = " << visit_to_location.first <<
                " location_id = " << visit_to_location.second << std::endl;
    }

    std::ofstream out_usr("visits_to_user.txt");
    for (const auto& visit_to_location : visits_to_locations_)
    {
        out_usr << "visit_id = " << visit_to_location.first <<
                " user_id = " << visit_to_location.second << std::endl;
    }

    std::ofstream out_loc_ser("locations.txt");
    for (const auto& location : locations_)
    {
        out_loc_ser << *location.second.Serialize() << std::endl;
    }

    std::ofstream out_usr_ser("users.txt");
    for (const auto& user : users_)
    {
        out_usr_ser << *user.second.Serialize() << std::endl;
    }
}
///