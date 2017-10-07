#include <sstream>

#include "User.h"

User::User(
        const uint32_t id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp birth_date)
    : Identifiable(id)
    , email_(email)
    , first_name_(first_name)
    , last_name_(last_name)
    , gender_(gender)
    , birth_date_(birth_date)
{
}

User::User()
    : Identifiable(0)
    , email_()
    , first_name_()
    , last_name_()
    , gender_()
    , birth_date_(0)
{
}

// User::User(
//         const rapidjson::Value& user_element)
//     : Identifiable(user_element["id"].GetInt())
//     , email_(user_element["email"].GetString())
//     , first_name_(user_element["first_name"].GetString())
//     , last_name_(user_element["last_name"].GetString())
//     , gender_(user_element["gender"].GetString() == "m" ? Gender::Male : Gender::Female)
//     , timestamp_(user_element["timestamp"].GetInt())
// {
// }

void User::Deserialize(
        const rapidjson::Value& user_element)
{
    id_ = user_element["id"].GetInt();
    email_ = user_element["email"].GetString();
    first_name_ = user_element["first_name"].GetString();
    last_name_ = user_element["last_name"].GetString();
    gender_ = user_element["gender"].GetString() == "m" ? Gender::Male : Gender::Female;
    birth_date_ = user_element["birth_date"].GetInt();
}

std::unique_ptr<std::string> User::Serialize() const
{
    std::stringstream str;

    std::string gender = gender_ == Gender::Male ? "m" : "f";

    str << "{\"id\":" << id_ <<
            ",\"email\":\"" << email_ <<
            "\",\"first_name\":\"" << first_name_ <<
            "\",\"last_name\":\"" << last_name_ <<
            "\",\"gender\":\"" << gender <<
            "\",\"birth_date\":" << birth_date_ << '}';

    return std::make_unique<std::string>(str.str());
}