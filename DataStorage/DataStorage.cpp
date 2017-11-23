#include <dirent.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>

#include "Macroses.h"
#include "DataStorage.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/filereadstream.h"
#include "../Submodules/rapidjson/include/rapidjson/document.h"
#include "../Submodules/zipper/zipper/unzipper.h"

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

    ///
//     DumpData();
    ///
}

std::vector<std::string> FileNames(
        const std::string& data_name,
        const std::string& pattern)
{
    zipper::Unzipper unzipper(data_name);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    std::vector<std::string> names;
    for (auto& e : entries)
    {
        if (e.name.find(std::string("data/FULL/data/") + pattern) == 0)
        {
            names.push_back(e.name);
        }
    }

    unzipper.close();

    return std::move(names);
}

std::vector<unsigned char> GetFileContent(
        const std::string& data_name,
        const std::string& filename)
{
    std::vector<unsigned char> unzipped_entry;
    zipper::Unzipper unzipper(data_name);
    unzipper.extractEntryToMemory(filename, unzipped_entry);
    unzipper.close();

    return unzipped_entry;
}

void DataStorage::InitializeUsers(
        const std::string& path_to_zipped_data)
{
  Trace("InitializeUsers");
  std::vector<std::string> file_names =
          FileNames(path_to_zipped_data, "users_");
  
  for (const auto& name : file_names)
  {
    std::vector<unsigned char> buffer =
            GetFileContent(path_to_zipped_data, name);
    rapidjson::Document d;
    d.Parse((char*)buffer.data(), buffer.size());
    rapidjson::Value& users = d["users"];

    for (auto& u : users.GetArray())
    {
        User user;
        user.Deserialize(u);
        users_.emplace(user.GetId(), user);
    }
  }
}

void DataStorage::InitializeLocations(
        const std::string& path_to_zipped_data)
{
    Trace("InitializeLocations");
    std::vector<std::string> file_names =
            FileNames(path_to_zipped_data ,"locations_");
  
    for (const auto& name : file_names)
    {
        std::vector<unsigned char> buffer =
                GetFileContent(path_to_zipped_data, name);
        rapidjson::Document d;
        d.Parse((char*)buffer.data(), buffer.size());
        rapidjson::Value& locations = d["locations"];

        for (auto& l : locations.GetArray())
        {
            Location location;
            location.Deserialize(l);
            locations_.emplace(location.GetId(), location);
        }
    }
}

void DataStorage::InitializeVisits(
        const std::string& path_to_zipped_data)
{
    Trace("InitializeVisits");
    std::vector<std::string> file_names =
            FileNames(path_to_zipped_data, "visits_");

    for (const auto& name : file_names)
    {
        std::vector<unsigned char> buffer =
                GetFileContent(path_to_zipped_data, name);
        rapidjson::Document d;
        d.Parse((char*)buffer.data(), buffer.size());
        rapidjson::Value& visits = d["visits"];

        for (auto& v : visits.GetArray())
        {
            Visit visit;
            visit.Deserialize(v);
            visits_.emplace(visit.GetId(), visit);
        }
    }
}

void DataStorage::LoadZippedData(
        const std::string& path_to_zipped_data)
{
    InitializeUsers(path_to_zipped_data);
    InitializeVisits(path_to_zipped_data);
    InitializeLocations(path_to_zipped_data);

    MapEntities();

    DumpData();
}

template <typename T>
void DataStorage::ParseFile(
        const std::string& file_path,
        const std::string& entities_name,
        Container<T>& container)
{
    using namespace rapidjson;

    FILE* file = fopen(file_path.c_str(), "r");
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
            locations_to_users_[location->first].emplace(user->second.birth_date, user->first);
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
        const uint32_t location_id) const
{
    return GetEntityById<Location>(location_id, locations_);
}

std::unique_ptr<std::string> DataStorage::GetUserById(
        const uint32_t user_id) const
{
    return GetEntityById<User>(user_id, users_);
}

std::unique_ptr<std::string> DataStorage::GetVisitById(
        const uint32_t visit_id) const
{
    return GetEntityById<Visit>(visit_id, visits_);
}

template <typename T>
std::unique_ptr<std::string> DataStorage::GetEntityById(
        const uint32_t entity_id,
        const Container<T>& entities) const
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
        // result += *visits_.find(visit_description.second)->second.Serialize();
        const auto visit = visits_.find(visit_description.second)->second;
        const auto location = locations_.find(visit.location_id_);

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                location != locations_.end());
        
        result += R"({"mark":)";
        result += std::to_string(visit.mark_);
        result += R"(,"visited_at":)";
        result += std::to_string(visit.visited_at_);
        result += R"(,"place":")";
        result += location->second.place_;
        result += R"("},)";
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
            comparator(current_user->second.birth_date, from_age_date))
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
            gender != user->second.gender)
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
//     return UpdateEntity<User>(user, users_);

    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(user.id_) != users_.end(),
            UpdateEntityStatus::EntityNotFound)

    auto& user_to_update =
                users_[user.id_];

    if (!user.email.empty())
    {
        user_to_update.email = user.email;    
    }

    if (!user.first_name.empty())
    {
        user_to_update.first_name = user.first_name;
    }

    if (!user.last_name.empty())
    {
        user_to_update.last_name = user.last_name;
    }

    if (user.gender != Gender::Any)
    {
        user_to_update.gender = user.gender;
    }

    if (user.birth_date != std::numeric_limits<Timestamp>::min())
    {
        user_to_update.birth_date = user.birth_date;
    }

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

DataStorage::UpdateEntityStatus DataStorage::UpdateVisit(
        const Visit& visit1)
{
    // May I recalc something?

    // MappedIndexes visits_to_locations_; // yes
    // MappedIndexes visits_to_user_; // yes
    // MappedMultiIndexes users_to_visits_; // yes
    // MappedMultiIndexes locations_to_visits_;
    // MappedMultiIndexes locations_to_users_;
    auto visit = visit1;

    ENSURE_TRUE_OTHERWISE_RETURN(
            visits_.find(visit.id_) != visits_.end(),
            UpdateEntityStatus::EntityNotFound);

    // dirty hack
    auto& visit_to_update = visits_[visit.id_];
    if (visit.location_id_ == std::numeric_limits<uint32_t>::max())
    {
        visit.location_id_ = visit_to_update.location_id_;
    }

    if (visit.user_id_ == std::numeric_limits<uint32_t>::max())
    {
        visit.user_id_ = visit_to_update.user_id_;
    }

    if (visit.visited_at_ == std::numeric_limits<Timestamp>::max())
    {
        visit.visited_at_ = visit_to_update.visited_at_;
    }

    if (visit.mark_ == std::numeric_limits<Mark>::max())
    {
        visit.mark_ = visit_to_update.mark_;
    }
    visits_[visit.id_] = visit;
    // dirty hack

    const auto visit_id_to_location =
            visits_to_locations_.find(visit.id_);
    ENSURE_TRUE_OTHERWISE_RETURN(
            visit_id_to_location != visits_to_locations_.end(),
            UpdateEntityStatus::EntityNotFound); // ???
    visits_to_locations_[visit.id_] = visit.location_id_;

    const auto visit_id_to_user =
            visits_to_user_.find(visit.id_);
    ENSURE_TRUE_OTHERWISE_RETURN(
            visit_id_to_user != visits_to_user_.end(),
            UpdateEntityStatus::EntityNotFound); // ???
    visits_to_user_[visit.id_] = visit.user_id_;

    ///
    const auto current_visit =
            visits_.find(visit.id_);

    ENSURE_TRUE_OTHERWISE_RETURN(
            current_visit != visits_.end(),
            UpdateEntityStatus::EntityNotFound);
    ///
    if (visit.user_id_ != current_visit->second.user_id_)
    {
        const auto user_id_to_visits =
                users_to_visits_.find(
                    current_visit->second.user_id_);
        // ENSURE
        const auto element_to_erase =
                user_id_to_visits->second.find(
                    current_visit->second.visited_at_);
        //ENSURE
        user_id_to_visits->second.erase(
                // current_visit->second.visited_at_);
                element_to_erase);
        
        users_to_visits_[visit.user_id_].emplace(
                visit.visited_at_,
                visit.id_);
    }
    else
    {

    }

    const auto location_id_to_visits =
            locations_to_visits_.find(
                current_visit->second.location_id_);
    ENSURE_TRUE_OTHERWISE_RETURN(
            location_id_to_visits != locations_to_visits_.end(),
            UpdateEntityStatus::EntityNotFound);
//     location_id_to_visits->second.erase(
//             location_id_to_visits->second.find(
//                 current_visit->second.visited_at_));

    const auto good_visits =
            location_id_to_visits->second.equal_range(
                current_visit->second.visited_at_);
    auto current_good_visit = good_visits.first;
    while (current_good_visit != good_visits.second)
    {
        if (current_good_visit->second == current_visit->first)
        {
            current_good_visit =
                    location_id_to_visits->second.erase(
                        current_good_visit);
        }
        else
        {
            ++current_good_visit;
        }
    }

    locations_to_visits_[visit.location_id_].emplace(
            visit.visited_at_,
            visit.id_);
    
    const auto location_id_to_users =
            locations_to_users_.find(current_visit->second.location_id_);
    // ENSURE
    auto user = users_.find(current_visit->second.user_id_);
    locations_to_users_[current_visit->second.location_id_].erase(user->second.birth_date);
    user = users_.find(visit.user_id_);
    if (user != users_.end())
    {
        locations_to_users_[visit.location_id_].emplace(
                user->second.birth_date,
                user->first);
    }
    else
    {
        // DebugTrace("if not (user != users_.end())");
    }
    
    // return UpdateEntity<Visit>(visit, visits_);
    return DataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated;
}

DataStorage::UpdateEntityStatus DataStorage::UpdateLocation(
        const Location& location)
{
    // May I recalc something?
//     return UpdateEntity<Location>(location, locations_);

    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id_) != locations_.end(),
            UpdateEntityStatus::EntityNotFound)

    auto& location_to_update =
                locations_[location.id_];

    if (location.distance_ != std::numeric_limits<uint64_t>::min())
    {
        location_to_update.distance_ = location.distance_;    
    }

    if (location.place_ != "")
    {
        location_to_update.place_ = location.place_;
    }

    if (location.country_ != "")
    {
        location_to_update.country_ = location.country_;
    }

    if (location.city_ != "")
    {
        location_to_update.city_ = location.city_;
    }

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

template <typename T>
DataStorage::UpdateEntityStatus DataStorage::UpdateEntity(
        const T& entity,
        Container<T>& entities)
{
    std::cout << "DataStorage::UpdateEntity" << std::endl;
    const auto entity_to_update = entities.find(entity.id_);
    std::cout << "DataStorage::UpdateEntity 2" << std::endl;

    ENSURE_TRUE_OTHERWISE_RETURN(
            entity_to_update != entities.end(),
            UpdateEntityStatus::EntityNotFound)

    std::cout << "DataStorage::UpdateEntity 3" << std::endl;
    entities[entity.id_] = entity;

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

DataStorage::AddEntityStatus DataStorage::AddUser(
        User&& user)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(user.id_) == users_.end(),
            DataStorage::AddEntityStatus::EntityAlreadyExist);

    users_.emplace(user.id_, user);

    // May be, it is not necessary

    // const auto user_id_to_visits =
    //         users_to_visits_.find(user.id_);
    // if (user_id_to_visits == users_to_visits_.end())
    // {
    //     users_to_visits_.emplace(
    //             user.id_,
    //             TimestampToId{ {user.birth_date_, user.id_} });
        
    // }
    // else
    // {
    //     ///
    //     // ???
    //     ///
    // }

    return AddEntityStatus::EntitySuccessfullyAdded;
}

DataStorage::AddEntityStatus DataStorage::AddVisit(
        Visit&& visit)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            visits_.find(visit.id_) == visits_.end(),
            DataStorage::AddEntityStatus::EntityAlreadyExist);

    visits_.emplace(visit.id_, visit);

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
        locations_to_visits_.emplace(
                visit.location_id_,
                TimestampToId{ {visit.visited_at_, visit.id_} });
    }
    else
    {
        location_id_to_visits->second.emplace(
                visit.visited_at_,
                visit.id_);
    }

    const auto user_id_to_visits =
            users_to_visits_.find(visit.user_id_);
    if (user_id_to_visits == users_to_visits_.end())
    {
        users_to_visits_.emplace(
                visit.user_id_,
                TimestampToId{ {visit.visited_at_, visit.id_} });
    }
    else
    {
        // Try to add enywhere - user can not be in two
        // locations in same time/
        user_id_to_visits->second.emplace(
                visit.visited_at_,
                visit.id_);
    }

    return AddEntityStatus::EntitySuccessfullyAdded;
}

DataStorage::AddEntityStatus DataStorage::AddLocation(
        Location&& location)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id_) == locations_.end(),
            DataStorage::AddEntityStatus::EntityAlreadyExist)
    locations_.emplace(location.id_,location);

    // const auto location_id_to_visits =
    //         locations_to_visits_.find(location.id_);
    // if (location_id_to_visits == locations_to_visits_.end())
    // {
    //     locations_to_visits_.emplace(
    //             location.id_,
    //             std::multimap<Timestamp,uint32_t>());
    // }
    // else
    // {
    //     ///
    //     // Do nothing???
    //     ///
    // }

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

    return DataStorage::AddEntityStatus::EntitySuccessfullyAdded;
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
    for (const auto& visit_to_user : visits_to_user_)
    {
        out_usr << "visit_id = " << visit_to_user.first <<
                " user_id = " << visit_to_user.second << std::endl;
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

    std::ofstream out_vis_ser("visits.txt");
    for (const auto& visit : visits_)
    {
        out_vis_ser << *visit.second.Serialize() << std::endl;
    }

    std::ofstream out_loc_to_vis_ser("locations_to_visits.txt");
    for (const auto& location_to_visits : locations_to_visits_)
    {
        for (const auto location_to_visit : location_to_visits.second)
        {
            out_loc_to_vis_ser << "location = " << location_to_visits.first <<
                    " visit = " << location_to_visit.second << std::endl;
        }
    }
}
///