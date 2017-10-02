#include "User.h"

User::User(
        const uint32_t id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp timestamp)
    : Identifiable(id)
    , email_(email)
    , first_name_(first_name)
    , last_name_(last_name)
    , gender_(gender)
    , timestamp_(timestamp)
{
}

User::User(
        const rapidjson::Value& user_element)
    : Identifiable(user_element["id"].GetInt())
    , email_(user_element["email"].GetString())
    , first_name_(user_element["first_name"].GetString())
    , last_name_(user_element["last_name"].GetString())
    , gender_(user_element["gender"].GetString() == "m" ? Gender::Male : Gender::Female)
    , timestamp_(user_element["timestamp"].GetInt())
{
}