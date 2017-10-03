#include <dirent.h>
// #include <experimental/filesystem>
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
    std::cout << "Container size = " << container.size() << std::endl;
    ///
}

void DataStorage::MapEntities()
{
    for (const auto& visit : visits_)
    {
        const auto location =
                locations_.find(visit.second.location_id_);
        const auto user =
                users_.find(visit.second.id_);
        if (location != locations_.end() &&
            user != users_.end())
        {
            visites_to_locations_.emplace(visit.first, location->first);
            visites_to_locations_.emplace(visit.first, user->first);
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
    return GetEntityById(location_id, EntityType::Location);
}

std::unique_ptr<std::string> DataStorage::GetEntityById(
        const uint32_t entity_id,
        EntityType entity_type)
{
    switch (entity_type)
    {
        case EntityType::Location:
        {
            const auto location = locations_.find(entity_id);
            
            return
                location == locations_.end()
                ? nullptr
                : std::make_unique<std::string>(*location->second.Serialize());
                
        } break;

        case EntityType::User:
        {

        } break;

        case EntityType::Visit:
        {

        } break;
    }
}

std::unique_ptr<std::string> DataStorage::GetVisistsByUserId(
        const uint32_t user_id,
        const Timestamp from_date,
        const Timestamp to_date,
        const std::string& country,
        const uint32_t to_distance)
{
    const auto range =
            users_to_visits_.equal_range(user_id);
    
    ENSURE_TRUE_OTHERWISE_RETURN(range.first != users_to_visits_.end(), nullptr)
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