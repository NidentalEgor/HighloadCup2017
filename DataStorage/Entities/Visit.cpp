#include <sstream>

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
    , location_id_(0)
    , user_id_(0)
    , visited_at_(0)
    , mark_(0)
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
