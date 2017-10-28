#include <sstream>

#include "User.h"

User::User()
    : Identifiable(0)
    , email()
    , first_name()
    , last_name()
    , gender()
    , birth_date(0)
{
}

User::User(
        const uint32_t id,
        const std::string& email,
        const std::string& first_name,
        const std::string& last_name,
        const Gender gender,
        const Timestamp birth_date)
    : Identifiable(id)
    , email(email)
    , first_name(first_name)
    , last_name(last_name)
    , gender(gender)
    , birth_date(birth_date)
{
}

void User::Deserialize(
        const rapidjson::Value& user_element)
{
    id_ = user_element["id"].GetInt();
    email = user_element["email"].GetString();
    first_name = user_element["first_name"].GetString();
    last_name = user_element["last_name"].GetString();
    gender =
        strcmp(user_element["gender"].GetString(), "m") == 0
        ? Gender::Male
        : Gender::Female;
    birth_date = user_element["birth_date"].GetInt();
}

std::unique_ptr<std::string> User::Serialize() const
{
    std::stringstream str;

    std::string user_gender = gender == Gender::Male ? "m" : "f";

    str << "{\"id\":" << id_ <<
            ",\"email\":\"" << email <<
            "\",\"first_name\":\"" << first_name <<
            "\",\"last_name\":\"" << last_name <<
            "\",\"gender\":\"" << user_gender <<
            "\",\"birth_date\":" << birth_date << '}';

    return std::make_unique<std::string>(str.str());
}
