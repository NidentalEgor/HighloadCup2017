#include <sstream>

#include "Macroses.h"

#include "Visit.h"

Visit::Visit(
        const uint32_t id,
        const uint32_t location_id,
        const uint32_t user_id,
        const Timestamp visited_at,
        const Mark mark)
    : Identifiable(id)
    , location_id_(location_id)
    , user_id_(user_id)
    , visited_at_(visited_at)
    , mark_(mark)
{
}

Visit::Visit()
    : Identifiable(0)
    , location_id_(std::numeric_limits<uint32_t>::max())
    , user_id_(std::numeric_limits<uint32_t>::max())
    , visited_at_(std::numeric_limits<Timestamp>::max())
    , mark_(std::numeric_limits<Mark>::max())
{
}

// Visit::Visit(
//         const rapidjson::Value& visit_element)
//     : Identifiable(visit_element["id"].GetInt())
//     , location_id_(visit_element["location"].GetInt())
//     , user_id_(visit_element["user"].GetInt())
//     , timestamp_(visit_element["visited_at"].GetInt())
//     , mark_(visit_element["mark"].GetInt())
// {
// }

bool Visit::operator<(const Visit& right) const
{
    return visited_at_ < right.visited_at_;
}

bool Visit::operator<(
        const Timestamp& right) const
{
    return visited_at_ < right;    
}

void Visit::Deserialize(
        const rapidjson::Value& visit_element)
{
    id_ = visit_element["id"].GetInt();
    location_id_ = visit_element["location"].GetInt();
    user_id_ = visit_element["user"].GetInt();
    visited_at_ = visit_element["visited_at"].GetInt();
    mark_ = visit_element["mark"].GetInt();
}

std::unique_ptr<std::string> Visit::Serialize() const
{
    std::stringstream str;
    
    str << "{\"id\":" << id_ <<
            ",\"location\":" << location_id_ <<
            ",\"user\":" << user_id_ <<
            ",\"visited_at\":" << visited_at_ <<
            ",\"mark\":" << mark_ << '}';

    return std::make_unique<std::string>(str.str());
}

bool Visit::Validate(
            const char* content) const
{
    rapidjson::Document json_content;;
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content.Parse(content).HasParseError(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("id") &&
                json_content["id"].IsUint(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("location") &&
                json_content["location"].IsUint(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("user") &&
                json_content["user"].IsUint(),
            false);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("visited_at") &&
                json_content["visited_at"].IsInt64(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("mark") &&
                json_content["mark"].IsUint64(),
            false);

    return true;
}