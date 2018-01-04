#include <dirent.h>
#include <sstream>
#include <iomanip>

#include "../Submodules/rapidjson/include/rapidjson/document.h"
#include "../Submodules/rapidjson/include/rapidjson/filereadstream.h"
#include "../Submodules/zipper/zipper/unzipper.h"

#include "../Utils/Macroses.h"
#include "IndexedDataStorage.h"

namespace
{

template<typename T>
struct AlwaysFalse : std::false_type {};

} // namespace

void IndexedDataStorage::LoadData(const std::string& folder_path)
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

    // MapEntities();

    ///
//     DumpData();
    ///
}

std::vector<std::string> FileNames(
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

void IndexedDataStorage::InitializeUsers(
        const std::string& path_to_zipped_data)
{
  Trace("InitializeUsers");
  std::vector<std::string> file_names =
          FileNames(path_to_zipped_data, "users_");
  
  for (const auto& name : file_names)
  {
    DebugTrace("Users file name = {}", name);
    std::vector<unsigned char> buffer =
            GetFileContent(path_to_zipped_data, name);
    rapidjson::Document d;
    d.Parse(reinterpret_cast<char*>(buffer.data()), buffer.size());
    rapidjson::Value& users = d["users"];

    for (auto& u : users.GetArray())
    {
        User user;
        user.Deserialize(u);
        users_.emplace(user.GetId(), user);
    }
  }
}

void IndexedDataStorage::InitializeLocations(
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
        d.Parse(reinterpret_cast<char*>(buffer.data()), buffer.size());
        rapidjson::Value& locations = d["locations"];

        for (auto& l : locations.GetArray())
        {
            Location location;
            location.Deserialize(l);
            locations_.emplace(location.GetId(), location);
        }
    }
}

void IndexedDataStorage::InitializeVisits(
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
        d.Parse(reinterpret_cast<char*>(buffer.data()), buffer.size());
        rapidjson::Value& visits = d["visits"];

        for (const auto& v : visits.GetArray())
        {
            Visit visit;
            visit.Deserialize(v);
            visits_.emplace(visit.GetId(), visit);
        }
    }
}

void IndexedDataStorage::LoadZippedData(
        const std::string& path_to_zipped_data)
{
    InitializeUsers(path_to_zipped_data);
    InitializeVisits(path_to_zipped_data);
    InitializeLocations(path_to_zipped_data);

    // MapEntities();

    // DumpData();
}

template <typename T>
void IndexedDataStorage::ParseFile(
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

void IndexedDataStorage::MapEntities()
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
            static size_t index = 0;
            // Trace("");
            // std::cout << "index = " << index++ << std::endl;
            visits_to_locations_.emplace(visit.first, location->first);
            locations_to_visits_[location->first].emplace(visit.second.visited_at, visit.first);
            locations_to_users_[location->first].emplace(user->second.birth_date, user->first);
            visits_to_user_.emplace(visit.first, user->first);
            users_to_visits_[user->first].emplace(visit.second.visited_at, visit.first);
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

std::unique_ptr<std::string> IndexedDataStorage::GetLocationById(
        const Id location_id) const
{
    return GetEntityById<Location>(location_id, locations_);
}

std::unique_ptr<std::string> IndexedDataStorage::GetUserById(
        const Id user_id) const
{
    return GetEntityById<User>(user_id, users_);
}

std::unique_ptr<std::string> IndexedDataStorage::GetVisitById(
        const Id visit_id) const
{
    return GetEntityById<Visit>(visit_id, visits_);
}

template <typename T>        
std::unique_ptr<std::string> IndexedDataStorage::GetEntityById(
        const Id entity_id)
{
    static_assert(
            AlwaysFalse<T>::value,
            "IndexedDataStorage::GetEntityById: unknown type.");
}

template <>
std::unique_ptr<std::string> IndexedDataStorage::GetEntityById<User>(
        const Id entity_id)
{
    return GetEntityById<User>(entity_id, users_);
}

template <>
std::unique_ptr<std::string> IndexedDataStorage::GetEntityById<Location>(
        const Id entity_id)
{
    return GetEntityById<Location>(entity_id, locations_);
}

template <>
std::unique_ptr<std::string> IndexedDataStorage::GetEntityById<Visit>(
        const Id entity_id)
{
    return GetEntityById<Visit>(entity_id, visits_);
}

template <typename T>
std::unique_ptr<std::string> IndexedDataStorage::GetEntityById(
        const Id entity_id,
        const Container<T>& entities) const
{
    const auto entity = entities.find(entity_id);

    return
        entity == entities.end()
        ? nullptr
        : std::make_unique<std::string>(*entity->second.Serialize());
}

void IndexedDataStorage::EraseByCountry(
        const std::string& country,
        std::multimap<Timestamp, Id>& visits) const
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

        ENSURE_TRUE_OTHERWISE_CONTINUE(
            location != locations_.end());

        if (location->second.country != country)
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        ++visit_description;
    }
}

void IndexedDataStorage::EraseByToDistance(
        const Distance to_distance,
        std::multimap<Timestamp, Id>& visits) const
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

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                location != locations_.end());

        if (location->second.distance >= to_distance)
        {
            visit_description = visits.erase(visit_description);
            continue;
        }

        ++visit_description;
    }
}

std::unique_ptr<std::string> IndexedDataStorage::GetVisistsByUserId(
        GetVisistsByUserIdQuery&& query_description) const
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

    if (query_description.to_distance != std::numeric_limits<Distance>::max())
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
        const auto location = locations_.find(visit.location_id);

        ENSURE_TRUE_OTHERWISE_CONTINUE(
                location != locations_.end());
        
        result += R"({"mark":)";
        result += std::to_string(visit.mark);
        result += R"(,"visited_at":)";
        result += std::to_string(visit.visited_at);
        result += R"(,"place":")";
        result += location->second.place;
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
void IndexedDataStorage::EraseGreaterOrEqualElements(
    const Timestamp bound,
    T& container) const
{
    container.erase(
            container.lower_bound(bound),
            container.end());
}

template <typename T>
void IndexedDataStorage::EraseLessOrEqualElements(
        const Timestamp bound,
        T& container) const
{
    container.erase(
            container.begin(),
            container.upper_bound(bound));
}

template <typename Comparator>
void IndexedDataStorage::EraseByAge(
        const Timestamp from_age,
        Comparator comparator,
        std::multimap<Timestamp, Id>& visits) const
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

void IndexedDataStorage::EraseByGender(
        const Gender gender,
        std::multimap<Timestamp, Id>& visits) const
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

std::unique_ptr<std::string> IndexedDataStorage::GetAverageLocationMark(
        GetAverageLocationMarkQuery&& query_description) const
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(query_description.id) != locations_.end(),
            nullptr);

    const auto location_idto_visits =
            locations_to_visits_.find(query_description.id);
    ENSURE_TRUE_OTHERWISE_RETURN(
            location_idto_visits != locations_to_visits_.end(),
            std::make_unique<std::string>(R"({"avg":0})"));

    auto visits = location_idto_visits->second;

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
        sum += visits_.find(visit.second)->second.mark;
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

Timestamp IndexedDataStorage::GetBoundaryBirthDate(
        const Age age) const
{
    static time_t now_epoch_time = time(nullptr);
    static struct tm* now_time = gmtime(&now_epoch_time);
    auto birth_date = *now_time;
    birth_date.tm_year -= age;
    time_t birth_date_epoch = mktime(&birth_date);

    return birth_date_epoch;
}

template<typename T>
IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateEntity(
        const T& entity)
{
    static_assert(
            AlwaysFalse<T>::value,
            "IndexedDataStorage::UpdateEntity: unknown type.");
}

template<>
IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateEntity<User>(
        const User& user)
{
    return UpdateUser(user);
}

template<>
IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateEntity<Location>(
        const Location& location)
{
    return UpdateLocation(location);
}

template<>
IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateEntity<Visit>(
        const Visit& visit)
{
    return UpdateVisit(visit);
}

IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateUser(
        const User& user)
{
    // May I recalc something?
//     return UpdateEntity<User>(user, users_);

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

IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateVisit(
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
            visits_.find(visit.id) != visits_.end(),
            UpdateEntityStatus::EntityNotFound);

    // dirty hack
    auto& visit_to_update = visits_[visit.id];
    if (visit.location_id == std::numeric_limits<Id>::max())
    {
        visit.location_id = visit_to_update.location_id;
    }

    if (visit.user_id == std::numeric_limits<Id>::max())
    {
        visit.user_id = visit_to_update.user_id;
    }

    if (visit.visited_at == std::numeric_limits<Timestamp>::max())
    {
        visit.visited_at = visit_to_update.visited_at;
    }

    if (visit.mark == std::numeric_limits<Mark>::max())
    {
        visit.mark = visit_to_update.mark;
    }
    // visits_[visit.id] = visit; // move to the end
    // dirty hack

    const auto visit_id_to_location =
            visits_to_locations_.find(visit.id);
    ENSURE_TRUE_OTHERWISE_RETURN(
            visit_id_to_location != visits_to_locations_.end(),
            UpdateEntityStatus::EntityNotFound); // ???
    visits_to_locations_[visit.id] = visit.location_id;

    const auto visit_id_to_user =
            visits_to_user_.find(visit.id);
    ENSURE_TRUE_OTHERWISE_RETURN(
            visit_id_to_user != visits_to_user_.end(),
            UpdateEntityStatus::EntityNotFound); // ???
    visits_to_user_[visit.id] = visit.user_id;

    ///
    const auto current_visit =
            visits_.find(visit.id);

    ENSURE_TRUE_OTHERWISE_RETURN(
            current_visit != visits_.end(),
            UpdateEntityStatus::EntityNotFound);
    ///
    if (visit.user_id != current_visit->second.user_id)
    {
        const auto user_id_to_visits =
                users_to_visits_.find(
                    current_visit->second.user_id);

        ENSURE_TRUE_OTHERWISE_RETURN(
                user_id_to_visits != users_to_visits_.end(),
                IndexedDataStorage::UpdateEntityStatus::EntityNotFound);

        const auto element_to_erase =
                user_id_to_visits->second.find(
                    current_visit->second.visited_at);
        
        ENSURE_TRUE_OTHERWISE_RETURN(
                element_to_erase != user_id_to_visits->second.end(),
                IndexedDataStorage::UpdateEntityStatus::EntityNotFound);

        user_id_to_visits->second.erase(
                // current_visit->second.visited_at);
                element_to_erase);
        
        users_to_visits_[visit.user_id].emplace(
                visit.visited_at,
                visit.id);
    }
    else
    {

    }

    const auto location_idto_visits =
            locations_to_visits_.find(
                current_visit->second.location_id);
    ENSURE_TRUE_OTHERWISE_RETURN(
            location_idto_visits != locations_to_visits_.end(),
            UpdateEntityStatus::EntityNotFound); // if no locations yet

    // ENSURE_TRUE_OTHERWISE_RETURN(
    //         location_idto_visits != locations_to_visits_.end(),
    //         UpdateEntityStatus::EntitySuccessfullyUpdated); // if no locations yet

    //     location_idto_visits->second.erase(
    //             location_idto_visits->second.find(
    //                 current_visit->second.visited_at));

    const auto good_visits =
            location_idto_visits->second.equal_range(
                current_visit->second.visited_at);
    auto current_good_visit = good_visits.first;
    while (current_good_visit != good_visits.second)
    {
        if (current_good_visit->second == current_visit->first)
        {
            current_good_visit =
                    location_idto_visits->second.erase(
                        current_good_visit);
        }
        else
        {
            ++current_good_visit;
        }
    }

    locations_to_visits_[visit.location_id].emplace(
            visit.visited_at,
            visit.id);
    
    const auto location_idto_users =
            locations_to_users_.find(current_visit->second.location_id);
    // ENSURE
    auto user = users_.find(current_visit->second.user_id);
    locations_to_users_[current_visit->second.location_id].erase(user->second.birth_date);
    user = users_.find(visit.user_id);
    if (user != users_.end())
    {
        locations_to_users_[visit.location_id].emplace(
                user->second.birth_date,
                user->first);
    }
    else
    {
    }
    
    visits_[visit.id] = visit;

    return IndexedDataStorage::UpdateEntityStatus::EntitySuccessfullyUpdated;
}

IndexedDataStorage::UpdateEntityStatus IndexedDataStorage::UpdateLocation(
        const Location& location)
{
    // May I recalc something?
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id) != locations_.end(),
            UpdateEntityStatus::EntityNotFound)

    auto& location_to_update =
                locations_[location.id];

    if (location.distance != std::numeric_limits<uint64_t>::min())
    {
        location_to_update.distance = location.distance;    
    }

    if (location.place != "")
    {
        location_to_update.place = location.place;
    }

    if (location.country != "")
    {
        location_to_update.country = location.country;
    }

    if (location.city != "")
    {
        location_to_update.city = location.city;
    }

    return UpdateEntityStatus::EntitySuccessfullyUpdated;
}

IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddUser(
        User&& user)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            users_.find(user.id) == users_.end(),
            IndexedDataStorage::AddEntityStatus::EntityAlreadyExist);

    users_.emplace(user.id, user);

    // May be, it is not necessary

    // const auto user_id_to_visits =
    //         users_to_visits_.find(user.id);
    // if (user_id_to_visits == users_to_visits_.end())
    // {
    //     users_to_visits_.emplace(
    //             user.id,
    //             TimestampToId{ {user.birth_date_, user.id} });
        
    // }
    // else
    // {
    //     ///
    //     // ???
    //     ///
    // }

    return AddEntityStatus::EntitySuccessfullyAdded;
}

IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddVisit(
        Visit&& visit)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            visits_.find(visit.id) == visits_.end(),
            IndexedDataStorage::AddEntityStatus::EntityAlreadyExist);

    visits_.emplace(visit.id, visit);

    visits_to_user_.emplace(
            visit.id,
            visit.user_id);
    visits_to_locations_.emplace(
            visit.id,
            visit.location_id);

    const auto location_idto_visits =
            locations_to_visits_.find(visit.location_id);
    if (location_idto_visits == locations_to_visits_.end())
    {
        locations_to_visits_.emplace(
                visit.location_id,
                TimestampToId{ {visit.visited_at, visit.id} });
    }
    else
    {
        location_idto_visits->second.emplace(
                visit.visited_at,
                visit.id);
    }

    const auto user_id_to_visits =
            users_to_visits_.find(visit.user_id);
    if (user_id_to_visits == users_to_visits_.end())
    {
        users_to_visits_.emplace(
                visit.user_id,
                TimestampToId{ {visit.visited_at, visit.id} });
    }
    else
    {
        // Try to add enywhere - user can not be in two
        // locations in same time
        user_id_to_visits->second.emplace(
                visit.visited_at,
                visit.id);
    }

    return AddEntityStatus::EntitySuccessfullyAdded;
}

IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddLocation(
        Location&& location)
{
    ENSURE_TRUE_OTHERWISE_RETURN(
            locations_.find(location.id) == locations_.end(),
            IndexedDataStorage::AddEntityStatus::EntityAlreadyExist)
    locations_.emplace(location.id,location);

    const auto location_idto_users =
            locations_to_users_.find(location.id);
    if (location_idto_users == locations_to_users_.end())
    {
        locations_to_users_.emplace(
                location.id,
                std::multimap<Timestamp, Id>());
    }
    else
    {
        ///
        // Do nothing???
        ///
    }

    return IndexedDataStorage::AddEntityStatus::EntitySuccessfullyAdded;
}

template<typename T>
IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddEntity(
        T&& entity)
{
    static_assert(
            AlwaysFalse<T>::value,
            "IndexedDataStorage::AddEntity: unknown type.");
}

template<>
IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddEntity<User>(
        User&& entity)
{
    return AddUser(std::forward<User>(entity));
}

template<>
IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddEntity<Location>(
        Location&& entity)
{
    return AddLocation(std::forward<Location>(entity));
}

template<>
IndexedDataStorage::AddEntityStatus IndexedDataStorage::AddEntity<Visit>(
        Visit&& entity)
{
    return AddVisit(std::forward<Visit>(entity));
}

///
#include <fstream>
void IndexedDataStorage::DumpData() const
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