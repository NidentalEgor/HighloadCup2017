#include "Visit.h"

Visit::Visit(
        const uint32_t id,
        const uint32_t location_id,
        const uint32_t user_id,
        const Timestamp timestamp,
        const Mark mark)
    : Identifiable(id)
    , location_id_(location_id)
    , user_id_(user_id)
    , timestamp_(timestamp)
    , mark_(mark)
{
}

Visit::Visit(
        const rapidjson::Value& visit_element)
    : Identifiable(visit_element["id"].GetInt())
    , location_id_(visit_element["location"].GetInt())
    , user_id_(visit_element["user"].GetInt())
    , timestamp_(visit_element["visited_at"].GetInt())
    , mark_(visit_element["mark"].GetInt())
{

}

Visit::Visit()
{
}
