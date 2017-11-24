#include <sstream>

#include "Macroses.h"

#include "Visit.h"

Visit::Visit(
        const Id id,
        const Id location_id,
        const Id user_id,
        const Timestamp visited_at,
        const Mark mark)
    : Identifiable(id)
    , location_id(location_id)
    , user_id(user_id)
    , visited_at(visited_at)
    , mark(mark)
{
}

Visit::Visit()
    : Identifiable(0)
    , location_id(std::numeric_limits<Id>::max())
    , user_id(std::numeric_limits<Id>::max())
    , visited_at(std::numeric_limits<Timestamp>::max())
    , mark(std::numeric_limits<Mark>::max())
{
}

void Visit::Deserialize(
        const rapidjson::Value& visit_element)
{
    id = visit_element["id"].GetInt();
    location_id = visit_element["location"].GetInt();
    user_id = visit_element["user"].GetInt();
    visited_at = visit_element["visited_at"].GetInt();
    mark = visit_element["mark"].GetInt();
}

std::unique_ptr<std::string> Visit::Serialize() const
{
    std::stringstream str;
    
    str << "{\"id\":" << id <<
            ",\"location\":" << location_id <<
            ",\"user\":" << user_id <<
            ",\"visited_at\":" << visited_at <<
            ",\"mark\":" << mark << '}';

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