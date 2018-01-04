#include <typeindex>
#include <dirent.h>
#include <sstream>
#include <iomanip>
#include <fstream>

#include "../Submodules/rapidjson/include/rapidjson/filereadstream.h"
#include "../Submodules/zipper/zipper/unzipper.h"
#include "../Utils/Macroses.h"

#include "DataStorage.h"

namespace
{

template <typename T>
struct AlwaysFalse : std::false_type {};

} // namespace

void DataStorageNew::LoadData(
        const std::string& folder_path)
{
    std::string folder_path_inner(folder_path);

    if (folder_path_inner.back() != '/')
    {
        folder_path_inner.push_back('/');
    }

    DIR* directory;
    struct dirent* entity;
    if ((directory = opendir(folder_path_inner.c_str())) != nullptr)
    {
        while ((entity = readdir (directory)) != nullptr)
        {
            const std::string file_path =
                    folder_path_inner + std::string(entity->d_name);
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

template <typename T>
void DataStorageNew::ParseFile(
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

std::map<std::type_index, std::string> entity_type_to_entity_name
{
    {typeid(User), "users_"},
    {typeid(Location), "locations_"},
    {typeid(Visit), "visits_"}
};

template <typename T>
void DataStorageNew::AddEntityIntoContainer(
        T&& entity)
{
    static_assert(
            AlwaysFalse<T>::value,
            "DataStorageNew::AddEntityIntoContainer: unknown type.");
}        

template <>
void DataStorageNew::AddEntityIntoContainer<std::pair<Id, User>>(
        std::pair<Id, User>&& entity)
{
    users_.emplace(std::forward<std::pair<Id, User>>(entity));
}

template <>
void DataStorageNew::AddEntityIntoContainer<std::pair<Id, Location>>(
        std::pair<Id, Location>&& entity)
{
    locations_.emplace(std::forward<std::pair<Id, Location>>(entity));
}

template <>
void DataStorageNew::AddEntityIntoContainer<std::pair<Id, Visit>>(
        std::pair<Id, Visit>&& entity)
{
    visits_.emplace(std::forward<std::pair<Id, Visit>>(entity));
}

std::vector<unsigned char> GetFileContentNew(
        const std::string& data_name,
        const std::string& filename)
{
    std::vector<unsigned char> unzipped_entry;
    zipper::Unzipper unzipper(data_name);
    unzipper.extractEntryToMemory(filename, unzipped_entry);
    unzipper.close();

    return unzipped_entry;
}

std::vector<std::string> FileNamesNew(
        const std::string& data_name,
        const std::string& pattern)
{
    // temp
    // if (is_pattern_big_)
    // {
    //     zipper::Unzipper unzipper(data_name);
    //     std::vector<zipper::ZipEntry> entries = unzipper.entries();
    //     std::vector<std::string> names;
    //     for (auto& e : entries)
    //     {
    //         if (e.name.find(std::string("data/FULL/data/") + pattern) == 0)
    //         {
    //             names.push_back(e.name);
    //         }
    //     }

    //     unzipper.close();

    //     return names;
    // }
    // temp

    zipper::Unzipper unzipper(data_name);
    std::vector<zipper::ZipEntry> entries = unzipper.entries();
    std::vector<std::string> names;
    for (const auto& e : entries)
    {
        if (e.name.find(pattern) == 0)
        {
            names.push_back(e.name);
        }
    }

    unzipper.close();

    return names;
}

template <typename T>
void DataStorageNew::InitializeEntity(
        const std::string& path_to_zipped_data)
{
    auto entity_name =
            entity_type_to_entity_name.find(typeid(T))->second;

    std::string string_for_trace("Initialize ");
    string_for_trace += entity_name;
    string_for_trace.pop_back();

    Trace(string_for_trace.c_str());
    
    std::vector<std::string> file_names =
            FileNamesNew(
                path_to_zipped_data,
                entity_name);
    
    entity_name.pop_back();
    for (const auto& name : file_names)
    {
        std::vector<unsigned char> buffer =
                GetFileContentNew(path_to_zipped_data, name);
        rapidjson::Document d;
        d.Parse(reinterpret_cast<char*>(buffer.data()), buffer.size());
        rapidjson::Value& entities = d[entity_name.c_str()];

        for (auto& e : entities.GetArray())
        {
            T entity;
            entity.Deserialize(e);
            AddEntityIntoContainer(
                    std::make_pair(entity.GetId(), entity));
        }
    }    
}

void DataStorageNew::MapEntities()
{
    Trace("Start mapping entities...");
    for (const auto& visit : visits_)
    {
        const auto location =
                locations_.find(visit.second.location_id);
        const auto user =
                users_.find(visit.second.user_id);
        if (location != locations_.end() &&
            user != users_.end())
        {
            users_to_visits_[user->first].emplace(visit.first);
            locations_to_visits_[location->first].emplace(visit.first);
        }
        else
        {
            ///
            // Error!!!
            ///
        }


    }
    Trace("Finish mapping entities...");
}

void DataStorageNew::LoadZippedData(
        const std::string& path_to_zipped_data)
{
    InitializeEntity<User>(path_to_zipped_data);
    InitializeEntity<Location>(path_to_zipped_data);
    InitializeEntity<Visit>(path_to_zipped_data);

    MapEntities();

    // DumpData();
}

std::unique_ptr<std::string> DataStorageNew::GetLocationById(
        const Id location_id) const
{
    const auto location = locations_.find(location_id);

    return
        location == locations_.end()
        ? nullptr
        : std::make_unique<std::string>(*location->second.Serialize());
}
   
std::unique_ptr<std::string> DataStorageNew::GetUserById(
        const Id user_id) const
{
    const auto user = users_.find(user_id);

    return
        user == users_.end()
        ? nullptr
        : std::make_unique<std::string>(*user->second.Serialize());
}
    
std::unique_ptr<std::string> DataStorageNew::GetVisitById(
        const Id visit_id) const
{
    const auto visit = visits_.find(visit_id);

    return
        visit == visits_.end()
        ? nullptr
        : std::make_unique<std::string>(*visit->second.Serialize());
}

// template <typename T>
// void EraseLessOrEqualElements(
//         const Timestamp bound,
//         T& container) const
// {
//     container.erase(
//             container.begin(),
//             container.upper_bound(bound));
// }

Timestamp DataStorageNew::GetBoundaryBirthDate(
        const Age age) const
{
    static time_t now_epoch_time = time(nullptr);
    static struct tm* now_time = gmtime(&now_epoch_time);
    auto birth_date = *now_time;
    birth_date.tm_year -= age;
    time_t birth_date_epoch = mktime(&birth_date);

    return birth_date_epoch;
}

std::unique_ptr<std::string> DataStorageNew::GetVisistsByUserId(
        GetVisistsByUserIdQuery&& query_description) const
{
    const auto current_user =
            users_.find(query_description.id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            current_user != users_.end(),
            nullptr);

    // naive implementation

    const auto user_to_visits =
            users_to_visits_.find(query_description.id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            user_to_visits != users_to_visits_.end(),
            std::make_unique<std::string>(R"({"visits":[]})"));

    std::multimap<Timestamp, Id> timestamp_to_visit_id;
    for (const auto visit_id : user_to_visits->second)
    {
        const auto visit =
                visits_.find(visit_id);

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                visit != visits_.end());        

        const auto location =
                locations_.find(visit->second.location_id);       

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                location != locations_.end());

        if (query_description.from_date != std::numeric_limits<Timestamp>::min())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    query_description.from_date < visit->second.visited_at);
        }

        if (query_description.to_date != std::numeric_limits<Timestamp>::max())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    query_description.to_date > visit->second.visited_at);
        }

        if (!query_description.country.empty())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    query_description.country == location->second.country);
        }
                
        if (query_description.to_distance != std::numeric_limits<Timestamp>::max())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    query_description.to_distance > location->second.distance);
        }

        timestamp_to_visit_id.emplace(
                visit->second.visited_at,
                visit->first);
    }

    std::string result(R"({"visits":[)");
    for (const auto visited_at_to_visit_id : timestamp_to_visit_id)
    {
        const auto visit =
                visits_.find(visited_at_to_visit_id.second);

        ENSURE_TRUE_OTHERWISE_CONTINUE( // may be not necessary???
                visit != visits_.end());
        
        const auto location =
                locations_.find(visit->second.location_id);

        ENSURE_TRUE_OTHERWISE_CONTINUE( // may be not necessary???
                location != locations_.end());

        // result += *visit->second.Serialize();
        
        result += "{";
        result += fmt::format(
                R"("place": "{}", "visited_at": {}, "mark": {})",
                location->second.place,
                visit->second.visited_at,
                visit->second.mark);        
        result += "},";
    }

    if (!timestamp_to_visit_id.empty())
    {
        result.pop_back();
    }

    result += R"(]})";

    return std::make_unique<std::string>(std::move(result));

    // naive implementation
}

std::unique_ptr<std::string> DataStorageNew::GetAverageLocationMark(
        GetAverageLocationMarkQuery&& query_description) const
{
    Timestamp from_date = std::numeric_limits<Timestamp>::min();
    Timestamp to_date = std::numeric_limits<Timestamp>::max();
    Timestamp from_age = std::numeric_limits<Timestamp>::min();
    Timestamp to_age = std::numeric_limits<Timestamp>::max();
    Gender gender = Gender::Any;

    const auto location =
            locations_.find(query_description.id);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            location != locations_.end(),
            nullptr);

    const auto location_to_visits =
            locations_to_visits_.find(query_description.id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            location_to_visits != locations_to_visits_.end(),
            std::make_unique<std::string>(R"({"avg":0})"));

    double marks_sum = 0.0;
    size_t marks_amount = 0;
    for (const auto visit_id : location_to_visits->second)
    {
        const auto visit =
                visits_.find(visit_id);

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                visit != visits_.end());

        if (query_description.from_date != std::numeric_limits<Timestamp>::min())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    visit->second.visited_at > query_description.from_date);
        }
                
        if (query_description.to_date != std::numeric_limits<Timestamp>::max())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    visit->second.visited_at < query_description.to_date);
        }

        const auto user =
                users_.find(visit->second.user_id);        

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                user != users_.end());

        if (query_description.gender != Gender::Any)
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                     user->second.gender == query_description.gender);
        }
        
        if (query_description.from_age != std::numeric_limits<Timestamp>::min())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    user->second.birth_date < GetBoundaryBirthDate(query_description.from_age));
                    // std::abs(std::abs(user->second.birth_date) - std::abs(GetBoundaryBirthDate(query_description.from_age))) < 31556926);
        }

        if (query_description.to_age != std::numeric_limits<Timestamp>::max())
        {
            ENSURE_TRUE_OTHERWISE_CONTINUE(
                    user->second.birth_date > GetBoundaryBirthDate(query_description.to_age));
                    // std::abs(std::abs(user->second.birth_date) - std::abs(GetBoundaryBirthDate(query_description.to_age))) < 31556926);
        }

        marks_sum += visit->second.mark;
        ++marks_amount;        
    }

    // std::string result(R"({"avg":)");
    // result +=
    //         fmt::format(
    //             "{}",
    //             marks_sum / static_cast<double>(marks_amount));
    // result += '}';
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            marks_amount != 0,
            std::make_unique<std::string>(R"({"avg":0})"));

    std::stringstream num;
    num << std::fixed << std::setprecision(5) << marks_sum / static_cast<double>(marks_amount);
    std::string result(R"({"avg":)");
    result += num.str();
    result += R"(})";
    // static std::string avg(R"({"avg":{}})");
    // return std::make_unique<std::string>(
    //         fmt::format(
    //             avg,
    //             marks_sum / static_cast<double>(marks_amount)));

    return std::make_unique<std::string>(std::move(result));
}
            
DataStorageNew::UpdateEntityStatus DataStorageNew::UpdateUser(
        const User& user)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(user.id) != users_.end(),
            UpdateEntityStatus::EntityNotFound)

    auto& user_to_update =
                users_[user.id];

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

DataStorageNew::UpdateEntityStatus DataStorageNew::UpdateLocation(
        const Location& location)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id) != locations_.end(),
            UpdateEntityStatus::EntityNotFound)

    auto& location_to_update =
                locations_[location.id];

    if (location.distance != std::numeric_limits<uint64_t>::min())
    {
        location_to_update.distance = location.distance;    
    }

    if (!location.place.empty())
    {
        location_to_update.place = location.place;
    }

    if (!location.country.empty())
    {
        location_to_update.country = location.country;
    }

    if (!location.city.empty())
    {
        location_to_update.city = location.city;
    }

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

DataStorageNew::UpdateEntityStatus DataStorageNew::UpdateVisit(
        const Visit& visit)
{
    const auto visit_to_update =
            visits_.find(visit.id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            visit_to_update != visits_.end(),
            UpdateEntityStatus::EntityNotFound);

    if (visit.location_id != std::numeric_limits<Id>::max() &&
        visit_to_update->second.location_id != visit.location_id)
    {
        const auto location_to_visits =
                locations_to_visits_.find(visit_to_update->second.location_id);

        if (location_to_visits != locations_to_visits_.end())
        {
            location_to_visits->second.erase(visit_to_update->second.id);
        }

        locations_to_visits_[visit.location_id].emplace(visit.id);

        visits_[visit.id].location_id = visit.location_id;
    }

    if (visit.user_id != std::numeric_limits<Id>::max() &&
        visit_to_update->second.user_id != visit.user_id)
    {
        const auto user_to_visits =
                users_to_visits_.find(visit_to_update->second.user_id);

        if (user_to_visits != users_to_visits_.end())
        {
            user_to_visits->second.erase(visit_to_update->second.user_id);
        }

        users_to_visits_[visit.user_id].emplace(visit.id);

        visits_[visit.id].user_id = visit.user_id;
    }

    if (visit.visited_at != std::numeric_limits<Timestamp>::max() &&
        visit_to_update->second.visited_at != visit.visited_at)
    {
        visits_[visit.id].visited_at = visit.visited_at;
    }

    if (visit.mark != std::numeric_limits<Mark>::max() &&
        visit_to_update->second.mark != visit.mark)
    {
        visits_[visit.id].mark = visit.mark;
    }

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

DataStorageNew::AddEntityStatus DataStorageNew::AddUser(
        User&& user)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(user.id) == users_.end(),
            IDataStorage::AddEntityStatus::EntityAlreadyExist);

    users_.emplace(user.id, user);

    return IDataStorage::AddEntityStatus::EntitySuccessfullyAdded;
}

DataStorageNew::AddEntityStatus DataStorageNew::AddLocation(
        Location&& location)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id) == locations_.end(),
            IDataStorage::AddEntityStatus::EntityAlreadyExist);

    locations_.emplace(location.id, location);

    return IDataStorage::AddEntityStatus::EntitySuccessfullyAdded;
}

DataStorageNew::AddEntityStatus DataStorageNew::AddVisit(
        Visit&& visit)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            visits_.find(visit.id) == visits_.end(),
            IDataStorage::AddEntityStatus::EntityAlreadyExist);

    visits_.emplace(visit.id, visit);

    users_to_visits_[visit.user_id].emplace(visit.id);
    locations_to_visits_[visit.location_id].emplace(visit.id);

    return IDataStorage::AddEntityStatus::EntitySuccessfullyAdded;
}

void DataStorageNew::DumpData() const
{
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

    std::ofstream users_to_visits("users_to_visits.txt");
    for (const auto& user_to_visits : users_to_visits_)
    {
        for (const auto visit_id : user_to_visits.second)
        {
            users_to_visits << "user id = " << user_to_visits.first <<
                    " visit id = " << visit_id << std::endl;
        }
    }

    std::ofstream locations_to_visits("locations_to_visits.txt");
    for (const auto& location_to_visits : locations_to_visits_)
    {
        for (const auto visit_id : location_to_visits.second)
        {
            users_to_visits << "location id = " << location_to_visits.first <<
                    " visit id = " << visit_id << std::endl;
        }
    }
}