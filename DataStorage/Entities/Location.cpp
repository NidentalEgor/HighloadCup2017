#include<sstream>
#include "Location.h"

Location::Location(
        const uint32_t id,
        const std::string& place,
        const std::string& country,
        const std::string& city,
        const uint32_t distance)
    : Identifiable(id)
    , place_(place)
    , country_(country)
    , city_(city)
    , distance_(distance)
{
}

Location::Location()
    : Identifiable(0)
    , place_()
    , country_()
    , city_()
    , distance_(0)
{

}

// Location::Location(
//         const rapidjson::Value& location_element)
//     : Identifiable(location_element["id"].GetInt())
//     , place_(location_element["place"].GetString())
//     , country_(location_element["country"].GetString())
//     , city_(location_element["city"].GetString())
//     , distance_(location_element["distance"].GetInt())
// {
// }

void Location::Deserialize(
        const rapidjson::Value& location_element)
{
    id_ = location_element["id"].GetInt();
    place_ = location_element["place"].GetString();
    country_ = location_element["country"].GetString();
    city_ = location_element["city"].GetString();
    distance_ = location_element["distance"].GetInt();
}

std::unique_ptr<std::string> Location::Serialize() const
{
    std::stringstream str;
    str << "{\"id\":" << id_ <<
            ",\"place\":\"" << place_ <<
            "\",\"country\":\"" << country_ <<
            "\",\"city\":\"" << city_ <<
            "\",\"distance\":" << distance_ << '}';

    return std::make_unique<std::string>(str.str());
}