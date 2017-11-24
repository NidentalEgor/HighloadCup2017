#include<sstream>

#include "../../Utils/Macroses.h"

#include "Location.h"

Location::Location(
        const Id id,
        const std::string& place,
        const std::string& country,
        const std::string& city,
        const Distance distance)
    : Identifiable(id)
    , place(place)
    , country(country)
    , city(city)
    , distance(distance)
{
}

Location::Location()
    : Identifiable(0)
    , place()
    , country()
    , city()
    , distance(std::numeric_limits<uint64_t>::min())
{

}

void Location::Deserialize(
        const rapidjson::Value& location_element)
{
    id_ = location_element["id"].GetInt();
    place = location_element["place"].GetString();
    country = location_element["country"].GetString();
    city = location_element["city"].GetString();
    distance = location_element["distance"].GetInt();
}

std::unique_ptr<std::string> Location::Serialize() const
{
    std::stringstream str;
    str << "{\"id\":" << id_ <<
            ",\"place\":\"" << place <<
            "\",\"country\":\"" << country <<
            "\",\"city\":\"" << city <<
            "\",\"distance\":" << distance << '}';

    return std::make_unique<std::string>(str.str());
}

bool Location::Validate(
        const char* content) const
{
    rapidjson::Document json_content;
    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content.Parse(content).HasParseError(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("id") &&
                json_content["id"].IsUint64(),
            false);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("place") &&
                json_content["place"].IsString(),
            false);
    
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("country") &&
                json_content["country"].IsString(),
            false);
            
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("city") &&
                json_content["city"].IsString(),
            false);

    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("distance") &&
                json_content["distance"].IsUint64(),
            false);
    
    return true;
}
