#include <dirent.h>
#include <iostream>

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
    if ((directory = opendir (folder_path.c_str())) != nullptr)
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

    ///
    std::cout << file_path << std::endl;
    ///
    
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

    ///
    // std::cout << "Container size = " << container.size() << std::endl;
    ///
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
            visites_to_locations_.emplace(visit.first, location->first);
            visites_to_users_.emplace(visit.first, user->first);
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

std::unique_ptr<std::string> DataStorage::GetVisistsByUserId(
        const uint32_t user_id,
        const Timestamp from_date,
        const Timestamp to_date,
        const std::string& country,
        const uint32_t to_distance)
{
    ENSURE_TRUE_OTHERWISE_RETURN(users_.find(user_id) != users_.end(), nullptr);

    const auto visits_iterator =
            users_to_visits_.find(user_id);
    
    ENSURE_TRUE_OTHERWISE_RETURN(visits_iterator != users_to_visits_.end(), nullptr);

    auto visits = visits_iterator->second;

    if (from_date != -1)
    {
        const auto first_suitable_visit =
                visits.upper_bound(from_date);

        visits.erase(visits.begin(), first_suitable_visit);
    }

    if (to_date != -1)
    {
        const auto last_suitable_visit =
                visits.upper_bound(to_date);

        visits.erase(last_suitable_visit, visits.end());
    }

    // Distance == 0?
    if (country != "" || to_distance != 0)
    {
        for (auto visit_description = visits.begin()
            ; visit_description != visits.end()
            ; ++visit_description)
        {
            // May be remove?
            const auto visit = visits_.find(visit_description->second);
            ENSURE_TRUE_OTHERWISE_CONTINUE(visit != visits_.end())
            //

            const auto location = locations_.find(visit->second.location_id_);
            ENSURE_TRUE_OTHERWISE_CONTINUE(location != locations_.end())

            if (country != "" && location->second.country_ != country)
            {
                visit_description =
                        visits.erase(visit_description);
            }

            // Distance == 0?
            if (to_distance != 0 && location->second.distance_ < to_distance)
            {
                visit_description =
                        visits.erase(visit_description);
            }
        }
    }

    std::string result(R"({"visits":[)");
    for (const auto visit_description : visits)
    {
        result += *visits_[visit_description.second].Serialize();
        result += ',';
    }

    if (!visits.empty())
    {
        result.pop_back();
    }

    result += "]}";
    return std::make_unique<std::string>(result);
}

///
#include <fstream>
void DataStorage::DumpData()
{
    std::ofstream out_loc("visits_to_locations.txt");
    for (const auto& visit_to_location : visites_to_locations_)
    {
        out_loc << "visit_id = " << visit_to_location.first <<
                " location_id = " << visit_to_location.second << std::endl;
    }

    std::ofstream out_usr("visits_to_user.txt");
    for (const auto& visit_to_location : visites_to_locations_)
    {
        out_usr << "visit_id = " << visit_to_location.first <<
                " user_id = " << visit_to_location.second << std::endl;
    }

    std::ofstream out_loc_ser("locations.txt");
    for (const auto& location : locations_)
    {
        out_loc_ser << *location.second.Serialize() << std::endl;
    }
}
///