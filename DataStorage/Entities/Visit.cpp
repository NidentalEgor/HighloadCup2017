#include <sstream>

#include "Macroses.h"

#include "Visit.h"

//
#include <iostream>
//

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

bool Visit::Validate(
            const char* content)
{
    std::cout << "content = " << content << std::endl;

    rapidjson::Document json_content;;

    ENSURE_TRUE_OTHERWISE_RETURN(
            !json_content.Parse(content).HasParseError(),
            false);

    // assert(document.IsObject()

    std::cout << "location" << std::endl;
    //Trace("location");
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("location") &&
                json_content["location"].IsUint(),
            false);

    std::cout << "user" << std::endl;
    //Trace("user");
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("user") &&
                json_content["user"].IsUint(),
            false);

    std::cout << "visited_at" << std::endl;
    //Trace("visited_at");
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("visited_at") &&
                json_content["visited_at"].IsInt64(),
            false);

    std::cout << "mark" << std::endl;
    //Trace("mark\n");
    ENSURE_TRUE_OTHERWISE_RETURN(
            json_content.HasMember("mark") &&
                json_content["visited_at"].IsUint64(),
            false);

    return true;
}